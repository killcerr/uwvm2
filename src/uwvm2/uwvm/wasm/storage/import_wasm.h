/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @brief       Imported wasm modules
 * @details     "--wasm-load-wasm" or "-Wlw"
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-28
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
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.uwvm.wasm.base;
import uwvm2.uwvm.wasm.feature;
#else
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::storage
{
    struct imported_wasm_t
    {
        ::fast_io::u8string_view import_wasm_module_name{};
        ::fast_io::native_file_loader import_wasm_file{};
        ::uwvm2::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t import_wasm_binfmt_ver1_storage{};
    };

    inline ::fast_io::vector<imported_wasm_t> imported_wasm{};  // [global] No global variable dependencies from other translation units
}  // namespace uwvm2::uwvm::wasm::storage
