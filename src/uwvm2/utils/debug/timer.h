/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-01
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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.utils.ansies;
# ifdef UWVM
import uwvm2.uwvm.io;
import uwvm2.uwvm.utils.ansies;
# endif
#else
// macro
# include <uwvm2/utils/macro/push_macros.h>
# ifdef UWVM
#  include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
# endif
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/utils/ansies/impl.h>
# ifdef UWVM
#  include <uwvm2/uwvm/io/impl.h>
#  include <uwvm2/uwvm/utils/ansies/impl.h>
# endif
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::debug
{
    struct timer
    {
        ::fast_io::u8string_view s{};
        ::fast_io::unix_timestamp t0{};
#ifdef __cpp_exceptions
        // `t0_success`: Only useful when exceptions are on, off exceptions directly crash the program.
        bool t0_success{true};
#endif

        // posix_clock_gettime may throw
        // Please use string literals initially to prevent dangling.

        UWVM_GNU_COLD inline explicit timer(::fast_io::u8string_view strvw) noexcept : s{strvw}
        {
#ifdef __cpp_exceptions
            try
#endif
            {
                t0 = ::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::monotonic_raw);
            }
#ifdef __cpp_exceptions
            catch(::fast_io::error)
            {
# ifdef UWVM
                if(::uwvm2::uwvm::show_vm_warning)
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        u8"[warn]  ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"timer can't get the time. ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                        u8"(vm)\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                }
# else
                ::fast_io::io::perr(::fast_io::u8err(), u8"uwvm: [warn]  timer can't get the time.\n");
# endif

                // Mark t0 as a failure and do not check t1 subsequently
                t0_success = false;
            }
#endif
        }

        inline timer(timer const&) = delete;
        inline timer& operator= (timer const&) = delete;
        inline timer(timer&&) = delete;
        inline timer& operator= (timer&&) = delete;

        UWVM_GNU_COLD inline ~timer()
        {
#ifdef __cpp_exceptions
            if(!t0_success) [[unlikely]]
            {
                // If the t0 fetch fails, processing is not continued to prevent outputting the wrong time.
                return;
            }
#endif

            // `t1` no initialization is required, even if subsequent exceptions are not used, there is no ub.
            ::fast_io::unix_timestamp t1;

#ifdef __cpp_exceptions
            try
#endif
            {
                t1 = ::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::monotonic_raw);
            }
#ifdef __cpp_exceptions
            catch(::fast_io::error)
            {
# ifdef UWVM
                if(::uwvm2::uwvm::show_vm_warning)
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        u8"[warn]  ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"timer can't get the time. ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                        u8"(vm)\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                }
# else
                ::fast_io::io::perr(::fast_io::u8err(), u8"uwvm: [warn]  timer can't get the time.\n");
# endif

                // An error message has been output and the return value cannot be obtained
                return;
            }
#endif
            // Output correct time
#ifdef UWVM
            ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"[debug] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"timer \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                s,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\": ",
                                t1 - t0,
                                u8"s\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
#else
            ::fast_io::io::perr(::fast_io::u8err(), u8"uwvm: [debug] timer \"", s, u8"\": ", t1 - t0, u8"s\n");
#endif
        }
    };

}  // namespace uwvm2::utils::debug

#ifndef UWVM_MODULE
// macro
# ifdef UWVM
#  include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# endif
# include <uwvm2/utils/macro/pop_macros.h>
#endif
