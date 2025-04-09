/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @brief       The wasm file that will be run
 * @details     "--run" or "-r"
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

module;

export module uwvm.wasm.storage:execute_wasm;

import fast_io;
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import uwvm.wasm.feature;

export namespace uwvm::wasm::storage
{
    inline ::fast_io::native_file_loader execute_wasm_file{};
    inline ::parser::wasm::base::mode execute_wasm_mode{::parser::wasm::base::mode::objdump};
    inline ::parser::wasm::base::abi execute_wasm_abi{::parser::wasm::base::abi::detect};
    inline ::parser::wasm::standard::wasm1::type::wasm_u32 execute_wasm_binfmt_ver{1u};

    // WASM Module Binfmt 1
    using wasm_binfmt1_features_t = decltype(::parser::wasm::concepts::operation::get_specified_binfmt_feature_tuple_from_all_freatures_tuple<1>(::uwvm::wasm::feature::features));
    inline constexpr wasm_binfmt1_features_t wasm_binfmt1_features{};
    using wasm_binfmt1_storage_t = decltype(::parser::wasm::concepts::operation::get_module_storage_type_from_tuple(wasm_binfmt1_features));
    inline wasm_binfmt1_storage_t execute_wasm_binfmt_ver1_storage{};
}  // namespace uwvm::wasm::storage
