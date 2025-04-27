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
#else
// std
# include <cstddef>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::base
{
    enum class wasm_parse_error_code : ::std::uint_least32_t
    {
        ok = 0u,
    };

    struct error_impl
    {
        ::std::byte const* err_curr{};
        /// @brief end_selectable can be nullptr
        ::std::byte const* err_end_selectable{}; 

        wasm_parse_error_code err_code{};
    };

}  // namespace parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
