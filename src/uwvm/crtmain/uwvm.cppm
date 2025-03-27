/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

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

module;

#include <cstdint>
#include <cstddef>

#include <utils/macro/push_macros.h>
#include <utils/ansies/ansi_push_macro.h>

/// @brief uwvm.crtmain:uwvm module declaration
export module uwvm.crtmain:uwvm;

import fast_io;
import uwvm.cmdline;
import uwvm.run;

/// @brief export uwvm namespace
export namespace uwvm
{
    /// @brief      uwvm c++ uz u8 main function
    /// @param      argc Argument Count
    /// @param      argv Argument Vector
    /// @return     exit value
    inline int uwvm_uz_u8main(::std::size_t argc, char8_t const* const* argv) noexcept
    {
        switch(::uwvm::cmdline::parsing(argc, argv))
        {
            case ::uwvm::cmdline::parsing_return_val::def: break;
            case ::uwvm::cmdline::parsing_return_val::return0: return 0;
            case ::uwvm::cmdline::parsing_return_val::returnm1: return -1;
            default: ::fast_io::unreachable();
        }

        ::uwvm::run::run();

        return 0;
    }

    /// @brief      uwvm c++ main function
    /// @param      argc Argument Count
    /// @param      argv Argument Vector
    /// @return     exit value
    /// @see        main()
    inline int uwvm_main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) noexcept
    {
#if defined(_WIN32) && !defined(_WIN32_WINDOWS)
        auto const argc_uz{::uwvm::cmdline::u8_cmdline.argc};
        auto const argv_u8{::uwvm::cmdline::u8_cmdline.argv.data()};
#else
        auto const argc_uz{static_cast<::std::size_t>(argc)};
        using char8_t_const_ptr_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const* const*;
        auto const argv_u8{reinterpret_cast<char8_t_const_ptr_const_may_alias_ptr>(argv)};
#endif
        return uwvm_uz_u8main(argc_uz, argv_u8);
    }

}  // namespace uwvm
