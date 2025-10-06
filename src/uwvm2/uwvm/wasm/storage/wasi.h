/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @brief       Imported wasm modules
 * @details     "--wasm-load-wasm" or "-Wlw"
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-28
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
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/object/memory/linear/impl.h>
# include <uwvm2/imported/wasi/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
# include <uwvm2/uwvm/wasm/type/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::storage
{
    using wasi_env_type = ::uwvm2::imported::wasi::wasip1::environment::wasip1_environment<::uwvm2::object::memory::linear::native_memory_t>;

    /// @brief     Default WasiPreview1 environment
    inline wasi_env_type default_wasi_env{};  // [global]

    /// @brief     Specialized WASM module WasiPreview1 environment
    /// @note      After processing all WASIs, perform specialization to ensure the WASIs' vectors no longer resize. You may first cache the parameters.
    inline ::uwvm2::utils::container::unordered_flat_map<::uwvm2::uwvm::wasm::type::all_module_t*, wasi_env_type>
        specialized_wasm_module_wasi_env{};  // [global]
}  // namespace uwvm2::uwvm::wasm::storage
