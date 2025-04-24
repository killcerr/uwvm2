/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-31
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

#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))

# ifdef UWVM_MODULE
import fast_io;
import utils.cmdline;
import utils.ansies;
# else
// std
#  include <memory>
// macro
#  include <utils/macro/push_macros.h>
#  include <utils/ansies/uwvm_color_push_macro.h>
// improt
#  include <fast_io.h>
#  include <fast_io_dsal/string_view.h>
#  include <utils/cmdline/impl.h>
#  include <utils/ansies/impl.h>
# endif

# ifndef UWVM_MODULE_EXPORT
#  define UWVM_MODULE_EXPORT
# endif

UWVM_MODULE_EXPORT namespace uwvm::cmdline::paras
{
    namespace details
    {
        inline constexpr ::fast_io::u8string_view log_win32_use_ansi_alias{u8"-ansi-log"};
    }  // namespace details

    inline constexpr ::utils::cmdline::parameter log_win32_use_ansi{
        .name{u8"--log-win32-use-ansi"},
        .describe{u8"Log on Win32 system using ANSI sequence under Win10. SetConsoleTextAttribute api is used by default."},
        .alias{::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::log_win32_use_ansi_alias), 1}},
        .is_exist{::std::addressof(::utils::ansies::log_win32_use_ansi_b)}};
}  // namespace uwvm::cmdline::paras

# ifndef UWVM_MODULE
// macro
#  include <utils/ansies/uwvm_color_pop_macro.h>
#  include <utils/macro/pop_macros.h>
# endif

#endif
