/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-27
 * @copyright   ASHP-1.0 License
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
// std
# include <source_location>
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
    [[noreturn]] inline void trap_and_inform_bug_pos(::std::source_location loc = ::fast_io::mnp::cur_src_loc()) noexcept
    {
#ifdef UWVM
        ::fast_io::io::panic(::fast_io::u8err(),
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                             u8"uwvm: ",
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                             u8"[debug] ",
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                             u8"There is a problem with the uwvm implementation: \n",
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                             loc,
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                             u8"\n\nPlease submit a bug report in the issue at (https://github.com/UlteSoft/uwvm2).\n",
                             ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
#else
        ::fast_io::io::panic(::fast_io::u8err(),
                             u8"uwvm: [debug] There is a problem with the uwvm implementation: \n",
                             loc,
                             u8"\n\nPlease submit a bug report in the issue at (https://github.com/UlteSoft/uwvm2).\n");
#endif
    }
}  // namespace uwvm2::utils::debug

#ifndef UWVM_MODULE
// macro
# ifdef UWVM
#  include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# endif
# include <uwvm2/utils/macro/pop_macros.h>
#endif
