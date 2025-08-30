/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @copyright   APL-2.0 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

#pragma once

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <climits>
# include <limits>
# include <memory>
# include <new>
# include <atomic>
# include <bit>
# include <utility>
// platform
# if !(defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                      \
                                                   (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))
#  include <sys/mman.h>
# endif
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/object/memory/wasm_page/impl.h>
# include <uwvm2/object/memory/platform_page/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

#if (defined(_WIN32) || defined(__CYGWIN__)) || (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                        \
                                                 (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))
# if CHAR_BIT != 8
#  error "mmap unsupported platform"
# endif
#endif

UWVM_MODULE_EXPORT namespace uwvm2::object::memory::linear
{
#if (defined(_WIN32) || defined(__CYGWIN__)) || (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                        \
                                                 (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))

    enum class mmap_memory_status_t : unsigned
    {
        wasm32,
        wasm64
    };

    // max_full_protection_wasm64_length not supported
    inline constexpr ::std::uint_least64_t max_partial_protection_wasm64_length{static_cast<::std::uint_least64_t>(1u) << 48u};  // 256 TB
    inline constexpr ::std::uint_least64_t max_full_protection_wasm32_length{static_cast<::std::uint_least64_t>(1u) << 33u};     // 8 GB
    inline constexpr ::std::uint_least32_t max_partial_protection_wasm32_length{static_cast<::std::uint_least32_t>(1u) << 28u};  // 256 MB

    struct mmap_memory_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        using allocator_t = ::fast_io::native_global_allocator;
        using atomic_size_allcator_t = ::fast_io::typed_generic_allocator_adapter<allocator_t, ::std::atomic_size_t>;

        ::std::byte* memory_begin{};
        ::std::atomic_size_t* memory_length_p{};
        // This memory_size is not used in the program unless custom_page is smaller than platform_page.
        // The number of platform pages can be obtained by rounding up the memory size to the nearest integer multiple of the platform page size.

        unsigned custom_page_size_log2{};

        mmap_memory_status_t status{};

        /// @brief This macro is used to control the behavior of the non-img compiler.
        inline static constexpr bool can_mmap{true};

        /// @brief When handling multithreaded growth, only the atomic variable `memory_length_p` is required. When `custom_page_size >= platform_page_size`,
        ///        dynamic determination of `memory_size` is unnecessary.
        inline static constexpr bool support_multi_thread{true};

        /// @brief      Default constructor.
        /// @note       The default page size is 65536 bytes.
        inline constexpr mmap_memory_t() noexcept
        {
            this->memory_length_p = atomic_size_allcator_t::allocate(1uz);
            ::new(this->memory_length_p)::std::atomic_size_t{};

            constexpr ::std::size_t default_wasm_page_size{65536uz};
            constexpr unsigned default_wasm_page_size_log2{static_cast<unsigned>(::std::countr_zero(default_wasm_page_size))};
            this->custom_page_size_log2 = default_wasm_page_size_log2;

            this->status = mmap_memory_status_t::wasm32;
        }

        inline constexpr mmap_memory_t(::std::size_t custom_page_size, mmap_memory_status_t memory_status) noexcept
        {
            this->memory_length_p = atomic_size_allcator_t::allocate(1uz);
            ::new(this->memory_length_p)::std::atomic_size_t{};

            // The same method as set_custom_page_size, but without adding a lock.

            // Check if it is a power of 2
            if(static_cast<unsigned>(::std::popcount(custom_page_size)) != 1u) [[unlikely]] { ::fast_io::fast_terminate(); }

            // Since there is only one pop, directly counting the number of zeros suffices to calculate log2
            this->custom_page_size_log2 = ::std::countr_zero(custom_page_size);

            this->status = memory_status;
        }

        inline constexpr bool require_dynamic_determination_memory_size() const noexcept
        {
            auto const [page_size, success]{::uwvm2::object::memory::platform_page::get_platform_page_size()};
            if(!success) [[unlikely]] { ::fast_io::fast_terminate(); }

            // UB will never appear; it has been preemptively checked.
            auto const custom_page_size{1uz << this->custom_page_size_log2};

            // When `custom_page_size` is less than one page, `mmap` can only guarantee the stability of the starting address and cannot provide page protection
            // checks. Additional dynamic verification is required.
            return custom_page_size < page_size;
        }

        inline constexpr bool is_full_page_protection() const noexcept
        {
            if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least64_t))
            {
                // 64bit platform
                switch(this->status)
                {
                    case mmap_memory_status_t::wasm32:
                    {
                        // full protection
                        return true;
                    }
                    case mmap_memory_status_t::wasm64:
                    {
                        // partial protection
                        return false;
                    }
                    [[unlikely]] default:
                    {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                        ::std::unreachable();
                    }
                }
            }
            else if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t))
            {
                // 32bit platform
                // both wasm64 and wasm32 are partial protection.
                return false;
            }
            else
            {
                static_assert(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t), "mmap unsupported platform");
            }
        }

        /// @brief      Initialize the memory.
        /// @note       Maximum value checks are not provided; maximum value checks should be performed outside of memory management.
        /// @note       You can use it after clear().
        inline constexpr void init_by_page_count(::std::size_t init_page_count) noexcept
        {
            ::std::size_t max_init_page_count;  // No initlization is required

            if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least64_t))
            {
                // 64bit platform
                switch(this->status)
                {
                    case mmap_memory_status_t::wasm32:
                    {
                        // In full protection mode, the maximum memory allocation permitted is half of the full protection size.
                        // ::std::numeric_limits<::std::uint_least32_t>::max() == max_full_protection_wasm32_length / 2u
                        static_assert(::std::numeric_limits<::std::uint_least32_t>::max() == max_full_protection_wasm32_length / 2u);
                        
                        max_init_page_count = ::std::numeric_limits<::std::uint_least32_t>::max() >> this->custom_page_size_log2;
                        break;
                    }
                    case mmap_memory_status_t::wasm64:
                    {
                        // partial protection
                        max_init_page_count = max_partial_protection_wasm64_length >> this->custom_page_size_log2;
                        break;
                    }
                    [[unlikely]] default:
                    {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                        ::std::unreachable();
                    }
                }
            }
            else if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t))
            {
                // 32bit platform
                // On 32-bit platforms, both wasm64 and wasm32 use 32-bit versions.
                // partial protection
                max_init_page_count = max_partial_protection_wasm32_length >> this->custom_page_size_log2;
            }
            else
            {
                static_assert(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t), "mmap unsupported platform");
            }

            // Page Protection
            if(init_page_count > max_init_page_count) [[unlikely]] { ::fast_io::fast_terminate(); }

            if(this->memory_begin == nullptr) [[likely]]
            {
                // UB will never appear; it has been preemptively checked.
                auto const custom_page_size{1uz << this->custom_page_size_log2};

                auto const memory_length{init_page_count * custom_page_size};

                this->memory_length_p.store(memory_length, ::std::memory_order_relaxed);

                // Reserve max space

                ::std::size_t max_protection_space;

                if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least64_t))
                {
                    // 64bit platform
                    switch(this->status)
                    {
                        case mmap_memory_status_t::wasm32:
                        {
                            // full protection
                            max_protection_space = max_full_protection_wasm32_length;
                            break;
                        }
                        case mmap_memory_status_t::wasm64:
                        {
                            // partial protection
                            max_protection_space = max_partial_protection_wasm64_length;
                            break;
                        }
                        [[unlikely]] default:
                        {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                            ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                            ::std::unreachable();
                        }
                    }
                }
                else if constexpr(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t))
                {
                    // 32bit platform
                    // On 32-bit platforms, both wasm64 and wasm32 use 32-bit versions.
                    // partial protection
                    max_protection_space = max_partial_protection_wasm32_length;
                }
                else
                {
                    static_assert(sizeof(::std::size_t) >= sizeof(::std::uint_least32_t), "mmap unsupported platform");
                }

                if(custom_page_size > ::std::numeric_limits<::std::size_t>::max() - max_protection_space) [[unlikely]] { ::fast_io::fast_terminate(); }

                // | max_protection_space | custom_page_size |
                auto const max_space{max_protection_space + custom_page_size};

# if defined(_WIN32) || defined(__CYGWIN__)                                                          // Windows
#  if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)  // WIN32
                {
                    auto const vamemory{reinterpret_cast<::std::byte*>(
                        ::fast_io::win32::VirtualAlloc(nullptr, max_space, 0x00002000u /*MEM_RESERVE*/, 0x01u /*PAGE_NOACCESS*/))};

                    if(vamemory == nullptr) [[unlikely]] { ::fast_io::fast_terminate(); }

                    this->memory_begin = vamemory;
                }
#  else  // NT
                {
                    // BaseAddress A pointer to a variable that will receive the base address of the allocated region of pages. If the initial value is
                    // not zero, the region is allocated at the specified virtual address
                    // This must be initialized to nullptr.
                    ::std::byte* vamemory{};

                    // Since const values cannot be used, a copy must be made.
                    ::std::size_t max_space_tmp{max_space};

                    constexpr bool zw{false};
                    auto const status{::fast_io::win32::nt::nt_allocate_virtual_memory<zw>(reinterpret_cast<void*>(-1),
                                                                                           reinterpret_cast<void**>(::std::addressof(vamemory)),
                                                                                           0u,
                                                                                           ::std::addressof(max_space_tmp),
                                                                                           0x00002000u /*MEM_RESERVE*/,
                                                                                           0x01u /*PAGE_NOACCESS*/)};

                    if(status) [[unlikely]] { ::fast_io::fast_terminate(); }

                    this->memory_begin = vamemory;
                }
#  endif
# elif !defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) && (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) &&   \
     __has_include(<sys/mman.h>)  // POSIX
                {
                    // The sys_mmap function in fast_io will throw an exception, but here it crashes directly with noexcept.
                    this->memory_begin = ::fast_io::details::sys_mmap(nullptr, max_space, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0u);
                }
# endif

                // Set pages according to the initialized size
                // The minimum allocation unit for mmap is a page. Even if you request a size smaller than a page, the kernel will ultimately manage and
                // map the memory in page-sized units.

# if defined(_WIN32) || defined(__CYGWIN__)                                                          // Windows
#  if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)  // WIN32
                {
                    auto const vamemory2{reinterpret_cast<::std::byte*>(
                        ::fast_io::win32::VirtualAlloc(this->memory_begin, memory_length, 0x00001000u /*MEM_COMMIT*/, 0x04u /*PAGE_READWRITE*/))};
                    if(vamemory2 != this->memory_begin) [[unlikely]] { ::fast_io::fast_terminate(); }
                }
#  else  // NT
                {
                    ::std::byte* vamemory2{this->memory_begin};
                    ::std::size_t memory_length_tmp{memory_length};
                    auto const status{::fast_io::win32::nt::nt_allocate_virtual_memory<zw>(reinterpret_cast<void*>(-1),
                                                                                           reinterpret_cast<void**>(::std::addressof(vamemory2)),
                                                                                           0,
                                                                                           ::std::addressof(memory_length_tmp),
                                                                                           0x00001000u /*MEM_COMMIT*/,
                                                                                           0x04u /*PAGE_READWRITE*/)};

                    if(status || vamemory2 != this->memory_begin) [[unlikely]] { ::fast_io::fast_terminate(); }
                }
#  endif
# elif !defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) && (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) &&   \
     __has_include(<sys/mman.h>)  // POSIX
                {
                    // The sys_mprotect function in fast_io will throw an exception (fast_io::error), but here it crashes directly with noexcept.
                    ::fast_io::details::sys_mprotect(this->memory_begin, memory_length, PROT_READ | PROT_WRITE);
                }
# endif
            }
        }

        /// @brief      Grow the memory.
        inline constexpr void grow(::std::size_t page_grow_size, ::std::size_t max_limit_memory_length = ::std::numeric_limits<::std::size_t>::max()) noexcept
        {
        }

        inline constexpr ::std::size_t get_page_size() const noexcept {}

        inline constexpr mmap_memory_t(mmap_memory_t const& other) noexcept = delete;

        inline constexpr mmap_memory_t& operator= (mmap_memory_t const& other) noexcept = delete;

        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done before the
        ///             WASM execution.
        inline constexpr mmap_memory_t(mmap_memory_t&& other) noexcept {}

        /// @note      This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done before the
        ///             WASM execution.
        inline constexpr mmap_memory_t& operator= (mmap_memory_t&& other) noexcept {}

        /// @brief      Clear the memory.
        /// @note       Simply clearing the memory without altering the page size.
        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline constexpr void clear() noexcept {}

        /// @brief      Clear the memory and destroy all flag.
        /// @note       After calling this, you must reinitialize it according to the initialization process.
        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline constexpr void clear_destroy() noexcept {}

        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline constexpr ~mmap_memory_t() noexcept {}
    };

#endif

}  // namespace uwvm2::object::memory::linear

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
#if (defined(_WIN32) || defined(__CYGWIN__)) || (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                        \
                                                 (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))
    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::object::memory::linear::mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::object::memory::linear::mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

#endif
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
