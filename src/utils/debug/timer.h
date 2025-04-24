/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-01
 * @copyright   APL-2 License
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

#ifdef UWVM_MODULE
import fast_io;
import utils.io;
import utils.ansies;
#else
// macro
# include <utils/macro/push_macros.h>
# include <utils/ansies/ansi_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <utils/io/impl.h>
# include <utils/ansies/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace utils::debug
{
    struct timer
    {
        ::fast_io::u8string_view s;
        ::fast_io::unix_timestamp t0;

        // posix_clock_gettime may throw
        UWVM_GNU_COLD inline explicit timer(::fast_io::u8string_view strvw) noexcept : s{strvw}
        {
#ifdef __cpp_exceptions
            try
#endif
            {
                t0 = ::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::monotonic_raw);
            }
#ifdef __cpp_exceptions
            catch(::fast_io::error e)
            {
                // do nothing
            }
#endif
        }

        inline timer(timer const&) = delete;
        inline timer& operator= (timer const&) = delete;
        inline timer(timer&&) = delete;
        inline timer& operator= (timer&&) = delete;

        UWVM_GNU_COLD inline ~timer()
        {
            ::fast_io::unix_timestamp t1{};

#ifdef __cpp_exceptions
            try
#endif
            {
                t1 = ::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::monotonic_raw);
            }
#ifdef __cpp_exceptions
            catch(::fast_io::error e)
            {
                // do nothing and return imme
                return;
            }
#endif
            ::fast_io::io::perr(
                ::utils::log_output,
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_white})
#else
                    UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE
#endif
                        ),
                u8"uwvm: ",
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_LT_CYAN,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_cyan})
#else
                    UWVM_AES_U8_LT_CYAN
#endif
                        ),
                u8"[debug] ",
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_WHITE,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_white})
#else
                    UWVM_AES_U8_WHITE
#endif
                        ),
                u8"timer \"",
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_YELLOW,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_yellow})
#else
                    UWVM_AES_U8_YELLOW
#endif
                        ),
                s,
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_WHITE,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_white})
#else
                    UWVM_AES_U8_WHITE
#endif
                        ),
                u8"\": ",
                t1 - t0,
                u8"s\n",
                ::fast_io::mnp::cond(
                    ::utils::ansies::put_color,
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                    ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,
                                         UWVM_AES_U8_RST_ALL,
                                         ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_ret_all})
#else
                    UWVM_AES_U8_RST_ALL
#endif
                        ));
        }
    };

}  // namespace utils::debug

#ifndef UWVM_MODULE
// macro
# include <utils/ansies/ansi_pop_macro.h>
# include <utils/macro/pop_macros.h>
#endif
