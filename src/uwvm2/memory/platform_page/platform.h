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
// platform
# if !(defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) &&                                      \
                                                   (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) && __has_include(<sys/mman.h>))
#  include <unistd.h>
# endif
// macro
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_push_macro.h>
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/uwvm_predefine/io/impl.h>
# include <uwvm2/uwvm_predefine/utils/ansies/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::memory::platform_page
{
    UWVM_GNU_PURE inline ::std::size_t get_platform_page_size() noexcept
    {
#if defined(_WIN32) || defined(__CYGWIN__)                                                          // Windows
# if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)  // WIN32

        ::fast_io::win32::system_info si;  // No initlization is required

        if(!::fast_io::win32::GetSystemInfo(::std::addressof(si))) [[unlikely]]
        {
            // Systems running in protected mode (with paging support) crash immediately upon a failed call.
#  ifdef UWVM
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Cannot get platform page size.\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
#  endif

            ::fast_io::fast_terminate();
        }

        return static_cast<::std::size_t>(si.dwPageSize);

# else  // NT

        ::fast_io::win32::nt::system_basic_information si;  // No initlization is required

        constexpr bool zw{false};
        auto const status{::fast_io::win32::nt::nt_query_system_information<zw>(::fast_io::win32::nt::system_information_class::SystemBasicInformation,
                                                                                ::std::addressof(si),
                                                                                sizeof(si),
                                                                                nullptr)};

        if(status) [[unlikely]]
        {
            // Systems running in protected mode (with paging support) crash immediately upon a failed call.
#  ifdef UWVM
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Cannot get platform page size.\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
#  endif

            ::fast_io::fast_terminate();
        }

        return static_cast<::std::size_t>(si.PageSize);

# endif
#elif !defined(__NEWLIB__) && !(defined(__MSDOS__) || defined(__DJGPP__)) && (!defined(__wasm__) || (defined(__wasi__) && defined(_WASI_EMULATED_MMAN))) &&    \
    __has_include(<sys/mman.h>)  // POSIX

        long const preferred_virtual_page_size{::fast_io::noexcept_call(::sysconf, _SC_PAGESIZE)};

        if(preferred_virtual_page_size == static_cast<long>(-1)) [[unlikely]]
        {
# ifdef UWVM
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Cannot get platform page size.\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
# endif

            ::fast_io::fast_terminate();
        }

        return static_cast<::std::size_t>(preferred_virtual_page_size);

#else  // None (e.g. i586-msdosdjgpp)

        return 0uz;

#endif
    }

}  // namespace uwvm2::memory::wasm_page

#ifndef UWVM_MODULE
// macro
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif
