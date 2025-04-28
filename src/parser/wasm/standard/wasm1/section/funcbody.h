/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-05
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
import ulte.parser.wasm.standard.wasm1.type;
#else
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::section
{
    /// @brief function bodys, use to storage
    struct code_func_body UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        ::fast_io::vector<::ulte::parser::wasm::standard::wasm1::type::local_entry> locals{};
        ::ulte::parser::wasm::standard::wasm1::type::vec_byte body{};
    };
}  // namespace ulte::parser::wasm::standard::wasm1::section

UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <>
    struct is_trivially_copyable_or_relocatable<::ulte::parser::wasm::standard::wasm1::section::code_func_body>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::ulte::parser::wasm::standard::wasm1::section::code_func_body>
    {
        inline static constexpr bool value = true;
    };
}  // namespace fast_io::freestanding

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
