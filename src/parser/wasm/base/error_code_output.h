/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-27
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
import :error_code;
#else
// std
# include <cstddef>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include "error_code.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::base
{
    struct error_output_flag_t
    {
        ::std::uint_least8_t enable_ansi : 1;
        ::std::uint_least8_t win32_use_text_attr : 1;  // on win95 - win7
    };

    struct error_output_t
    {
        ::std::byte const* module_begin{};
        ::parser::wasm::base::error_impl err{};
        error_output_flag_t flag{};
    };

}  // namespace parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
