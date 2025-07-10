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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm_custom.customs;
import uwvm2.uwvm.wasm.base;
import uwvm2.uwvm.wasm.feature;
#else
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm_custom/customs/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::type
{
    struct wasm_file_t
    {
        // Accurate module names that must work
        ::fast_io::u8string_view module_name{};
        // Memory-mapped or memory-copy (for platforms that don't support memory mapping) open wasm files
        ::fast_io::native_file_loader wasm_file{};
        // Module parsing results
        ::uwvm2::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t wasm_binfmt_ver1_storage{};
        // (Optional) Module name + symbol name
        ::uwvm2::parser::wasm_custom::customs::name_storage_t wasm_binfmt_ver1_name{};
    };

}  // namespace uwvm2::uwvm::wasm::storage
