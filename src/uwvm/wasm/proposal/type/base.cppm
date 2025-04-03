/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-03
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
#include <concepts>
#include <bit>

#include <uwvm/wasm/feature/feature_push_macro.h>

export module uwvm.wasm.proposal.type:base;

import fast_io;

export namespace uwvm::wasm::proposal::type
{
    /// @brief f16, bf16 and f128: future🦄
    /// @details
#if defined(UWVM_WASM_SUPPORT_F16)
    using wasm_f16 = __float16;
#endif

#if defined(UWVM_WASM_SUPPORT_BF16)
    using wasm_bf16 = decltype(0.0bf16);
#endif

#if defined(UWVM_WASM_SUPPORT_F128)
    using wasm_f128 = __float128;
#endif

}  // namespace uwvm::wasm::proposal::type
