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
# include <uwvm2/object/memory/wasm_page/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::object::memory::linear
{
#if (defined(_WIN32) || defined(__CYGWIN__)) || (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                        \
                                                 (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))

    struct mmap_memory_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        using allocator_t = ::fast_io::native_global_allocator;
        using atomic_size_allcator_t = ::fast_io::typed_generic_allocator_adapter<allocator_t, ::std::atomic_size_t>;

        ::std::byte* memory_begin;
        ::std::atomic_size_t* memory_size_p;
        // This memory_size is not used in the program unless custom_page is smaller than platform_page.
        // The number of platform pages can be obtained by rounding up the memory size to the nearest integer multiple of the platform page size.

        unsigned custom_page_size_log2{};

        /// @brief This macro is used to control the behavior of the non-img compiler.
        inline static constexpr bool can_mmap{true};

        /// @brief When handling multithreaded growth, only the atomic variable `memory_size_p` is required. When `custom_page_size >= platform_page_size`,
        ///        dynamic determination of `memory_size` is unnecessary.
        inline static constexpr bool support_multi_thread{true};
    };

#endif

}  // namespace uwvm2::object::memory::wasm_page

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
