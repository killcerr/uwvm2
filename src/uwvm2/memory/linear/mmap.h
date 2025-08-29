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

#if defined(_WIN32) || defined(__CYGWIN__)                                                          // Windows
# if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)  // WIN32

    struct win32_mmap_memory_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        ::std::byte* memory_begin;
        ::std::atomic_size_t* memory_size_p;
    };

# else  // NT

    template <::fast_io::nt_family family>
    struct basic_nt_mmap_memory_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
    };

    using nt_mmap_memory_t = basic_nt_mmap_memory_t<::fast_io::nt_family::nt>;
    using zw_mmap_memory_t = basic_nt_mmap_memory_t<::fast_io::nt_family::zw>;

# endif
#elif !defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) && (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) &&    \
    __has_include(<sys/mman.h>)  // POSIX

    struct posix_mmap_memory_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
    };

#endif

}  // namespace uwvm2::memory::wasm_page

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
#if defined(_WIN32) || defined(__CYGWIN__)                                                          // Windows
# if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)  // WIN32

    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::memory::linear::win32_mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::memory::linear::win32_mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

# else  // NT

    template <::fast_io::nt_family family>
    struct is_trivially_copyable_or_relocatable<::uwvm2::memory::linear::basic_nt_mmap_memory_t<family>>
    {
        inline static constexpr bool value = true;
    };

    template <::fast_io::nt_family family>
    struct is_zero_default_constructible<::uwvm2::memory::linear::basic_nt_mmap_memory_t<family>>
    {
        inline static constexpr bool value = true;
    };

# endif
#elif !defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) && (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) &&    \
    __has_include(<sys/mman.h>)  // POSIX

    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::memory::linear::posix_mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::memory::linear::posix_mmap_memory_t>
    {
        inline static constexpr bool value = true;
    };

#endif
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
