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
# include <limits>
# include <memory>
# include <new>
# include <atomic>
# include <bit>
# include <utility>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/memory/wasm_page/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::memory::linear
{
    /// @brief      The guard for the growing flag.
    /// @note       This guard is only used in `grow`. During memory operations, it directly employs `this->growing_flag->wait(true,
    ///             std::memory_order_relaxed);`.
    struct growing_flag_guard_t
    {
        ::std::atomic_flag* growing_flag_p{};

        UWVM_ALWAYS_INLINE inline constexpr growing_flag_guard_t(::std::atomic_flag* other_growing_flag_p) noexcept : growing_flag_p{other_growing_flag_p}
        {
            if(this->growing_flag_p != nullptr) [[likely]]
            {
                while(this->growing_flag_p->test_and_set(::std::memory_order_acquire)) { this->growing_flag_p->wait(true, ::std::memory_order_relaxed); }
            }
        }

        inline constexpr growing_flag_guard_t(growing_flag_guard_t const& other) noexcept = delete;
        inline constexpr growing_flag_guard_t& operator= (growing_flag_guard_t const& other) noexcept = delete;
        inline constexpr growing_flag_guard_t(growing_flag_guard_t&& other) noexcept = delete;
        inline constexpr growing_flag_guard_t& operator= (growing_flag_guard_t&& other) noexcept = delete;

        UWVM_ALWAYS_INLINE inline constexpr ~growing_flag_guard_t() noexcept
        {
            if(this->growing_flag_p != nullptr) [[likely]]
            {
                this->growing_flag_p->clear(::std::memory_order_release);
                // Memory access for all threads has been blocked, so it is necessary to notify all.
                this->growing_flag_p->notify_all();
            }
        }
    };

    /// @brief      The basic allocator memory class.
    /// @defailt    The allocator memory supports the following scenarios: First, platforms that do not support mmap. Second, when the size of custom_page is
    ///             smaller than platform_page.
    template <typename Alloc>
    struct basic_allocator_memory_t
    {
        using allocator_t = Alloc;
        using atomic_flag_allcator_t = ::fast_io::native_typed_global_allocator<::std::atomic_flag>;

        // data
        ::std::byte* memory_begin{};
        ::std::size_t memory_length{};

        unsigned custom_page_size_log2{};
        ::std::atomic_flag* growing_flag_p{};

        // function

        /// @brief      Default constructor.
        /// @note       The default page size is 65536 bytes.
        inline constexpr basic_allocator_memory_t() noexcept
        {
            this->growing_flag_p = atomic_flag_allcator_t::allocate(1uz);
            ::new(this->growing_flag_p)::std::atomic_flag{ATOMIC_FLAG_INIT};

            constexpr ::std::size_t default_wasm_page_size{65536uz};
            constexpr unsigned default_wasm_page_size_log2{static_cast<unsigned>(::std::countr_zero(default_wasm_page_size))};
            this->custom_page_size_log2 = default_wasm_page_size_log2;
        }

        inline constexpr basic_allocator_memory_t(::std::size_t custom_page_size) noexcept
        {
            this->growing_flag_p = atomic_flag_allcator_t::allocate(1uz);
            ::new(this->growing_flag_p)::std::atomic_flag{ATOMIC_FLAG_INIT};

            set_custom_page_size(custom_page_size);
        }

        inline constexpr void set_custom_page_size(::std::size_t custom_page_size) noexcept
        {
            growing_flag_guard_t growing_flag_guard{this->growing_flag_p};

            // Check if it is a power of 2
            if(static_cast<unsigned>(::std::popcount(custom_page_size)) != 1u) [[unlikely]] { ::fast_io::fast_terminate(); }

            // Since there is only one pop, directly counting the number of zeros suffices to calculate log2
            this->custom_page_size_log2 = ::std::countr_zero(custom_page_size);
        }

        /// @brief      Initialize the memory.
        /// @note       Maximum value checks are not provided; maximum value checks should be performed outside of memory management.
        inline constexpr void init_by_page_count(::std::size_t init_page_count) noexcept
        {
            if(init_page_count > ::std::numeric_limits<::std::size_t>::max() >> this->custom_page_size_log2) [[unlikely]] { ::fast_io::fast_terminate(); }

            growing_flag_guard_t growing_flag_guard{this->growing_flag_p};

            if(this->memory_begin == nullptr) [[likely]]
            {
                // UB will never appear; it has been preemptively checked.
                auto const custom_page_size{1uz << this->custom_page_size_log2};

                this->memory_length = init_page_count * custom_page_size;

                // Ensure alignment. Typically, the maximum allowed alignment size for WASM memory operation instructions is 16 (v128). Here, align to the size
                // of a cache line, which is usually 64.
                constexpr ::std::size_t alignment{64uz};

                auto const temp_memory_begin{allocator_t::allocate_aligned_zero(alignment, this->memory_length)};

                if(reinterpret_cast<::std::size_t>(temp_memory_begin) % alignment != 0uz) [[unlikely]] { ::fast_io::fast_terminate(); }

                this->memory_begin = ::std::assume_aligned<alignment>(reinterpret_cast<::std::byte*>(temp_memory_begin));
            }
        }

        /// @brief      Grow the memory.
        inline constexpr void grow(::std::size_t page_grow_size, ::std::size_t max_limit_memory_length = ::std::numeric_limits<::std::size_t>::max()) noexcept
        {
            if(page_grow_size == 0uz) [[unlikely]] { return; }

            growing_flag_guard_t growing_flag_guard{this->growing_flag_p};

            if(this->memory_begin == nullptr) [[unlikely]] { ::fast_io::fast_terminate(); }

            auto const custom_page_size{1uz << this->custom_page_size_log2};

            if(page_grow_size > ::std::numeric_limits<::std::size_t>::max() / custom_page_size) [[unlikely]]
            {
                // This situation cannot occur; it is due to user input error.
                ::fast_io::fast_terminate();
            }

            auto const memory_grow_size{page_grow_size * custom_page_size};

            if(max_limit_memory_length < this->memory_length) [[unlikely]]
            {
                // This situation cannot occur; it is due to user input error.
                ::fast_io::fast_terminate();
            }

            auto const left_memory_size{max_limit_memory_length - this->memory_length};

            if(memory_grow_size > left_memory_size) [[unlikely]]
            {
                // Exceeded the maximum allowed value, error reported.
                ::fast_io::fast_terminate();
            }

            this->memory_length += memory_grow_size;

            this->memory_begin = reinterpret_cast<::std::byte*>(allocator_t::reallocate_zero(this->memory_begin, this->memory_length));
        }

        inline constexpr ::std::size_t get_page_size() const noexcept
        {
            growing_flag_guard_t growing_flag_guard{this->growing_flag_p};
            // UB will never appear; it has been preemptively checked.
            auto const custom_page_size{1uz << this->custom_page_size_log2};
            return this->memory_length / custom_page_size;
        }

        inline constexpr basic_allocator_memory_t(basic_allocator_memory_t const& other) noexcept = delete;

        inline constexpr basic_allocator_memory_t& operator= (basic_allocator_memory_t const& other) noexcept = delete;

        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done before the
        ///             WASM execution.
        inline constexpr basic_allocator_memory_t(basic_allocator_memory_t&& other) noexcept
        {
            this->memory_begin = other.memory_begin;
            this->memory_length = other.memory_length;
            this->custom_page_size_log2 = other.custom_page_size_log2;
            this->growing_flag_p = other.growing_flag_p;

            // clear destory other
            other.memory_begin = nullptr;
            other.memory_length = 0uz;
            other.custom_page_size_log2 = 0u;
            other.growing_flag_p = nullptr;
        }

        /// @note      This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done before the
        ///             WASM execution.
        inline constexpr basic_allocator_memory_t& operator= (basic_allocator_memory_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            clear_destroy();

            this->memory_begin = other.memory_begin;
            this->memory_length = other.memory_length;
            this->custom_page_size_log2 = other.custom_page_size_log2;
            this->growing_flag_p = other.growing_flag_p;

            // clear destory other
            other.memory_begin = nullptr;
            other.memory_length = 0uz;
            other.custom_page_size_log2 = 0u;
            other.growing_flag_p = nullptr;

            return *this;
        }

        /// @brief      Clear the memory.
        /// @note       Simply clearing the memory without altering the page size.
        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline constexpr void clear() noexcept
        {
            Alloc::deallocate_n(this->memory_begin, this->memory_length);  // dealloc includes built-in nullptr checking

            this->memory_length = 0uz;
            this->memory_begin = nullptr;
        }

        /// @brief      Clear the memory and destroy the growing flag.
        /// @note       After calling this, you must reinitialize it according to the initialization process.
        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline constexpr void clear_destroy() noexcept
        {
            Alloc::deallocate_n(this->memory_begin, this->memory_length);  // dealloc includes built-in nullptr checking

            if(this->growing_flag_p != nullptr) [[likely]] { ::std::destroy_at(this->growing_flag_p); }

            atomic_flag_allcator_t::deallocate_n(this->growing_flag_p, 1uz);  // dealloc includes built-in nullptr checking

            this->memory_length = 0uz;
            this->memory_begin = nullptr;
            this->custom_page_size_log2 = 0u;
            this->growing_flag_p = nullptr;
        }

        /// @note       This function is designed to be lock-free and cannot be executed during WASM execution (multi-threaded). It can only be done after the
        ///             WASM execution.
        inline ~basic_allocator_memory_t() noexcept
        {
            Alloc::deallocate_n(this->memory_begin, this->memory_length);  // dealloc includes built-in nullptr checking

            if(this->growing_flag_p != nullptr) [[likely]] { ::std::destroy_at(this->growing_flag_p); }

            atomic_flag_allcator_t::deallocate_n(this->growing_flag_p, 1uz);  // dealloc includes built-in nullptr checking

            // multiple call to destructor is undefined behavior, so never set to nullptr
        }
    };

    using allocator_memory_t = basic_allocator_memory_t<::fast_io::native_global_allocator>;

}  // namespace uwvm2::memory::wasm_page

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <typename Alloc>
    struct is_trivially_copyable_or_relocatable<::uwvm2::memory::linear::basic_allocator_memory_t<Alloc>>
    {
        inline static constexpr bool value = true;
    };

    template <typename Alloc>
    struct is_zero_default_constructible<::uwvm2::memory::linear::basic_allocator_memory_t<Alloc>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
