/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-24
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

/// @brief      The following are the macros used by ansi escape sequences.
/// @details    Use `pop_macro` to avoid side effects on existing macros. Please use `pop_macro` in conjunction.

// #pragma once

#pragma pop_macro("WASM_WIN32_COLOR_WHITE")
#pragma pop_macro("WASM_WIN32_COLOR_LT_CYAN")
#pragma pop_macro("WASM_WIN32_COLOR_LT_PURPLE")
#pragma pop_macro("WASM_WIN32_COLOR_LT_BLUE")
#pragma pop_macro("WASM_WIN32_COLOR_YELLOW")
#pragma pop_macro("WASM_WIN32_COLOR_LT_GREEN")
#pragma pop_macro("WASM_WIN32_COLOR_LT_RED")
#pragma pop_macro("WASM_WIN32_COLOR_DK_GRAY")
#pragma pop_macro("WASM_WIN32_COLOR_GRAY")
#pragma pop_macro("WASM_WIN32_COLOR_CYAN")
#pragma pop_macro("WASM_WIN32_COLOR_PURPLE")
#pragma pop_macro("WASM_WIN32_COLOR_BLUE")
#pragma pop_macro("WASM_WIN32_COLOR_ORANGE")
#pragma pop_macro("WASM_WIN32_COLOR_GREEN")
#pragma pop_macro("WASM_WIN32_COLOR_RED")
#pragma pop_macro("WASM_WIN32_COLOR_BLACK")
#pragma pop_macro("WASM_WIN32_COLOR_RST_ALL_AND_SET_WHITE")
#pragma pop_macro("WASM_WIN32_COLOR_RST_ALL")

#include <ulte/utils/ansies/ansi_pop_macro.h>
