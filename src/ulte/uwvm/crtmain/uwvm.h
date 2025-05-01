﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @file        uwvm.cppm
 * @brief       uwvm cpp main function
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-20
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
import ulte.uwvm.crtmain.global;
import ulte.uwvm.cmdline;
import ulte.uwvm.run;
#else
// std
# include <cstdint>
# include <cstddef>
// macro
# include <ulte/utils/macro/push_macros.h>
# include <ulte/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include "global/impl.h"
# include <ulte/uwvm/cmdline/impl.h>
# include <ulte/uwvm/run/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace ulte::uwvm
{
    /// @brief      uwvm c++ uz u8 main function
    /// @param      argc Argument Count
    /// @param      argv Argument Vector
    /// @return     exit value
    inline int uwvm_uz_u8main(::std::size_t argc, char8_t const* const* argv) noexcept
    {
        switch(::ulte::uwvm::cmdline::parsing(argc, argv))
        {
            case ::ulte::uwvm::cmdline::parsing_return_val::def: break;
            case ::ulte::uwvm::cmdline::parsing_return_val::return0: return 0;
            case ::ulte::uwvm::cmdline::parsing_return_val::returnm1: return -1;  // Invalid parameters
            default: ::fast_io::unreachable();
        }

        return ::ulte::uwvm::run::run();
    }

    /// @brief      uwvm c++ main function
    /// @details    Handles initialization of all global variables. Only non-raii global variables, or constexpr raii global variables, are allowed, rtti global
    ///             variables and constinit raii global variables are forbidden.
    /// @param      argc Argument Count
    /// @param      argv Argument Vector
    /// @return     exit value
    /// @see        main()
    inline int uwvm_main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) noexcept
    {
        // program manipulation
        // Generate guard to protect global pod pointer
#if (defined(_WIN32) && !defined(__CYGWIN__)) && !defined(_WIN32_WINDOWS)
        // set win32 console output and windows cp to utf8
        ::ulte::uwvm::global::set_win32_console_io_cp_to_utf8 set_native_console_io_cp_to_utf8_ele{};
        // set win32 console ansi escape
        ::ulte::uwvm::global::enable_win32_ansi enable_native_ansi_ele{};
#endif
        // Automatically get the correct timezone data
        ::ulte::uwvm::global::tz_set_s tz_set_ele{};
#if defined(_WIN32) && !defined(_WIN32_WINDOWS)
        // codecvt parameters on winnt
        ::ulte::uwvm::cmdline::nt_code_cvt_argv_storage const u8_cmdline{::ulte::uwvm::cmdline::nt_code_cvt_argv()};
        auto const argc_uz{u8_cmdline.argc};
        auto const argv_u8{u8_cmdline.argv.data()};
#else
        auto const argc_uz{static_cast<::std::size_t>(argc)};
        using char8_t_const_ptr_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const* const*;
        auto const argv_u8{reinterpret_cast<char8_t_const_ptr_const_may_alias_ptr>(argv)};
#endif
        // u8main
        return uwvm_uz_u8main(argc_uz, argv_u8);
    }

}  // namespace ulte::uwvm

#ifndef UWVM_MODULE
// macro
# include <ulte/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <ulte/utils/macro/pop_macros.h>
#endif
