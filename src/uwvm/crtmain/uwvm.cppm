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

/// @brief uwvm.crtmain:uwvm module declaration
export module uwvm.crtmain:uwvm;

import fast_io;

import utils.io;

import uwvm.custom;

/// @brief export uwvm namespace
export namespace uwvm
{
    /// @brief      uwvm c++ main function
    /// @param      argc Argument Count
    /// @param      argv Argument Vector
    /// @return     exit value
    /// @see        main()
    inline int uwvm_main(int argc, char const* const* argv) noexcept
    {
        auto const argc_uz{static_cast<::std::size_t>(argc)};
        using char8_t_const_ptr_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const* const*;
        auto const argv_u8{reinterpret_cast<char8_t_const_ptr_const_may_alias_ptr>(argv)};

        return 0;
    }
}  // namespace uwvm
