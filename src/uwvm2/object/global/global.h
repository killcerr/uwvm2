/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
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

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <limits>
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1p1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm3/type/impl.h>
# include "ref.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::object::global
{
    enum class global_type : unsigned
    {
        wasm_i32,
        wasm_i64,
        wasm_f32,
        wasm_f64,
        wasm_v128,
        wasm_ref
    };

    union wasm_global_storage_u
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_i32 i32;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_i64 i64;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_f32 f32;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_f64 f64;
        ::uwvm2::parser::wasm::standard::wasm1p1::type::wasm_v128 v128;
        ::uwvm2::object::global::wasm_global_ref_t ref;
    };

    struct wasm_global_storage_t
    {
        wasm_global_storage_u storage;
        ::uwvm2::object::global::global_type kind;
        bool is_mutable;
    };

}  // namespace uwvm2::object::global

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
