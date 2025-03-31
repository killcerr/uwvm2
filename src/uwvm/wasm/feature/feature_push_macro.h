/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-01
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

#pragma push_macro("UWVM_WASM_SUPPORT_F16")
#undef UWVM_WASM_SUPPORT_F16
#if defined(__SIZEOF_FLOAT16__) || defined(__FLOAT16__)
# define UWVM_WASM_SUPPORT_F16
#endif

#pragma push_macro("UWVM_WASM_SUPPORT_BF16")
#undef UWVM_WASM_SUPPORT_BF16
#if defined(__STDCPP_BFLOAT16_T__)
# define UWVM_WASM_SUPPORT_BF16
#endif

#pragma push_macro("UWVM_WASM_SUPPORT_F128")
#undef UWVM_WASM_SUPPORT_F128
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
# define UWVM_WASM_SUPPORT_F128
#endif