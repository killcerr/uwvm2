﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @brief       Imported wasm modules
 * @details     "--wasm-load-wasm" or "-Wlw"
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-28
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
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.uwvm.wasm.base;
import ulte.uwvm.wasm.feature;
#else
// import
# include <fast_io.h>
# include <ulte/parser/wasm/concepts/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
# include <ulte/uwvm/wasm/base/impl.h>
# include <ulte/uwvm/wasm/feature/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace ulte::uwvm::wasm::storage
{
    inline ::fast_io::vector<::fast_io::native_file_loader> import_wasm_file{};  // [global] No global variable dependencies from other translation units
    inline ::fast_io::vector<::ulte::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t>
        import_wasm_binfmt_ver1_storage{};  // [global] No global variable dependencies from other translation units
    // TODO: import_wasm_module // The result of parsing the wasm is used to add the import module directly.
}  // namespace ulte::uwvm::wasm::storage
