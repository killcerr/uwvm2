/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

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
/// @details    Use `push_macro` to avoid side effects on existing macros. Please use `pop_macro` in conjunction.

// #pragma once

#include <utils/ansies/ansi_push_macro.h>

#pragma push_macro("WASM_WIN32_COLOR_RST_ALL")
#undef WASM_WIN32_COLOR_RST_ALL
#define WASM_WIN32_COLOR_RST_ALL(stream)                                                                                                                       \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_rst_all }

#pragma push_macro("WASM_WIN32_COLOR_RST_ALL_AND_SET_WHITE")
#undef WASM_WIN32_COLOR_RST_ALL_AND_SET_WHITE
#define WASM_WIN32_COLOR_RST_ALL_AND_SET_WHITE(stream)                                                                                                         \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_white }

#pragma push_macro("WASM_WIN32_COLOR_BLACK")
#undef WASM_WIN32_COLOR_BLACK
#define WASM_WIN32_COLOR_BLACK(stream)                                                                                                                         \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_black }

#pragma push_macro("WASM_WIN32_COLOR_RED")
#undef WASM_WIN32_COLOR_RED
#define WASM_WIN32_COLOR_RED(stream)                                                                                                                           \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_red }

#pragma push_macro("WASM_WIN32_COLOR_GREEN")
#undef WASM_WIN32_COLOR_GREEN
#define WASM_WIN32_COLOR_GREEN(stream)                                                                                                                         \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_green }

#pragma push_macro("WASM_WIN32_COLOR_ORANGE")
#undef WASM_WIN32_COLOR_ORANGE
#define WASM_WIN32_COLOR_ORANGE(stream)                                                                                                                        \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_orange }

#pragma push_macro("WASM_WIN32_COLOR_BLUE")
#undef WASM_WIN32_COLOR_BLUE
#define WASM_WIN32_COLOR_BLUE(stream)                                                                                                                          \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_blue }

#pragma push_macro("WASM_WIN32_COLOR_PURPLE")
#undef WASM_WIN32_COLOR_PURPLE
#define WASM_WIN32_COLOR_PURPLE(stream)                                                                                                                        \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_purple }

#pragma push_macro("WASM_WIN32_COLOR_CYAN")
#undef WASM_WIN32_COLOR_CYAN
#define WASM_WIN32_COLOR_CYAN(stream)                                                                                                                          \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_cyan }

#pragma push_macro("WASM_WIN32_COLOR_GRAY")
#undef WASM_WIN32_COLOR_GRAY
#define WASM_WIN32_COLOR_GRAY(stream)                                                                                                                          \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_gray }

#pragma push_macro("WASM_WIN32_COLOR_DK_GRAY")
#undef WASM_WIN32_COLOR_DK_GRAY
#define WASM_WIN32_COLOR_DK_GRAY(stream)                                                                                                                       \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_dk_gray }

#pragma push_macro("WASM_WIN32_COLOR_LT_RED")
#undef WASM_WIN32_COLOR_LT_RED
#define WASM_WIN32_COLOR_LT_RED(stream)                                                                                                                        \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_lt_red }

#pragma push_macro("WASM_WIN32_COLOR_LT_GREEN")
#undef WASM_WIN32_COLOR_LT_GREEN
#define WASM_WIN32_COLOR_LT_GREEN(stream)                                                                                                                      \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_lt_green }

#pragma push_macro("WASM_WIN32_COLOR_YELLOW")
#undef WASM_WIN32_COLOR_YELLOW
#define WASM_WIN32_COLOR_YELLOW(stream)                                                                                                                        \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_yellow }

#pragma push_macro("WASM_WIN32_COLOR_LT_BLUE")
#undef WASM_WIN32_COLOR_LT_BLUE
#define WASM_WIN32_COLOR_LT_BLUE(stream)                                                                                                                       \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_lt_blue }

#pragma push_macro("WASM_WIN32_COLOR_LT_PURPLE")
#undef WASM_WIN32_COLOR_LT_PURPLE
#define WASM_WIN32_COLOR_LT_PURPLE(stream)                                                                                                                     \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_lt_purple }

#pragma push_macro("WASM_WIN32_COLOR_LT_CYAN")
#undef WASM_WIN32_COLOR_LT_CYAN
#define WASM_WIN32_COLOR_LT_CYAN(stream)                                                                                                                       \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_lt_cyan }

#pragma push_macro("WASM_WIN32_COLOR_WHITE")
#undef WASM_WIN32_COLOR_WHITE
#define WASM_WIN32_COLOR_WHITE(stream)                                                                                                                         \
    ::ulte::utils::ansies::win32_text_attr { stream, ::ulte::utils::ansies::text_attr::foreground_white }

