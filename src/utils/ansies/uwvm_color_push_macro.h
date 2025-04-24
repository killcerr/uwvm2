/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
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

#include "ansi_push_macro.h"

#pragma push_macro("UWVM_COLOR_U8_RST_ALL")
#undef UWVM_COLOR_U8_RST_ALL
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_RST_ALL                                                                                                                                    \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_RST_ALL,                                                                                                                 \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_rst_all})
#else
# define UWVM_COLOR_U8_RST_ALL UWVM_AES_U8_RST_ALL
#endif

#pragma push_macro("UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE")
#undef UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE                                                                                                                      \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE,                                                                                               \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_white})
#else
# define UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE
#endif

#pragma push_macro("UWVM_COLOR_U8_BLACK")
#undef UWVM_COLOR_U8_BLACK
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_BLACK                                                                                                                                      \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_BLACK,                                                                                                                   \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_black})
#else
# define UWVM_COLOR_U8_BLACK UWVM_AES_U8_BLACK
#endif

#pragma push_macro("UWVM_COLOR_U8_RED")
#undef UWVM_COLOR_U8_RED
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_RED                                                                                                                                        \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_RED,                                                                                                                     \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_red})
#else
# define UWVM_COLOR_U8_RED UWVM_AES_U8_RED
#endif

#pragma push_macro("UWVM_COLOR_U8_GREEN")
#undef UWVM_COLOR_U8_GREEN
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_GREEN                                                                                                                                      \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_GREEN,                                                                                                                   \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_green})
#else
# define UWVM_COLOR_U8_GREEN UWVM_AES_U8_GREEN
#endif

#pragma push_macro("UWVM_COLOR_U8_ORANGE")
#undef UWVM_COLOR_U8_ORANGE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_ORANGE                                                                                                                                     \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_ORANGE,                                                                                                                  \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_orange})
#else
# define UWVM_COLOR_U8_ORANGE UWVM_AES_U8_ORANGE
#endif

#pragma push_macro("UWVM_COLOR_U8_BLUE")
#undef UWVM_COLOR_U8_BLUE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_BLUE                                                                                                                                       \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_BLUE,                                                                                                                    \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_blue})
#else
# define UWVM_COLOR_U8_BLUE UWVM_AES_U8_BLUE
#endif
#pragma push_macro("UWVM_COLOR_U8_PURPLE")
#undef UWVM_COLOR_U8_PURPLE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_PURPLE                                                                                                                                     \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_PURPLE,                                                                                                                  \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_purple})
#else
# define UWVM_COLOR_U8_PURPLE UWVM_AES_U8_PURPLE
#endif

#pragma push_macro("UWVM_COLOR_U8_CYAN")
#undef UWVM_COLOR_U8_CYAN
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_CYAN                                                                                                                                       \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_CYAN,                                                                                                                    \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_cyan})
#else
# define UWVM_COLOR_U8_CYAN UWVM_AES_U8_CYAN
#endif

#pragma push_macro("UWVM_COLOR_U8_GRAY")
#undef UWVM_COLOR_U8_GRAY
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_GRAY                                                                                                                                       \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_GRAY,                                                                                                                    \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_gray})
#else
# define UWVM_COLOR_U8_GRAY UWVM_AES_U8_GRAY
#endif

#pragma push_macro("UWVM_COLOR_U8_DK_GRAY")
#undef UWVM_COLOR_U8_DK_GRAY
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_DK_GRAY                                                                                                                                    \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_DK_GRAY,                                                                                                                 \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_dk_gray})
#else
# define UWVM_COLOR_U8_DK_GRAY UWVM_AES_U8_DK_GRAY
#endif

#pragma push_macro("UWVM_COLOR_U8_LT_RED")
#undef UWVM_COLOR_U8_LT_RED
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_LT_RED                                                                                                                                     \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_LT_RED,                                                                                                                  \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_red})
#else
# define UWVM_COLOR_U8_LT_RED UWVM_AES_U8_LT_RED
#endif

#pragma push_macro("UWVM_COLOR_U8_LT_GREEN")
#undef UWVM_COLOR_U8_LT_GREEN
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_LT_GREEN                                                                                                                                   \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_LT_GREEN,                                                                                                                \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_green})
#else
# define UWVM_COLOR_U8_LT_GREEN UWVM_AES_U8_LT_GREEN
#endif

#pragma push_macro("UWVM_COLOR_U8_YELLOW")
#undef UWVM_COLOR_U8_YELLOW
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_YELLOW                                                                                                                                     \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_YELLOW,                                                                                                                  \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_yellow})
#else
# define UWVM_COLOR_U8_YELLOW UWVM_AES_U8_YELLOW
#endif

#pragma push_macro("UWVM_COLOR_U8_LT_BLUE")
#undef UWVM_COLOR_U8_LT_BLUE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_LT_BLUE                                                                                                                                    \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_LT_BLUE,                                                                                                                 \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_blue})
#else
# define UWVM_COLOR_U8_LT_BLUE UWVM_AES_U8_LT_BLUE
#endif

#pragma push_macro("UWVM_COLOR_U8_LT_PURPLE")
#undef UWVM_COLOR_U8_LT_PURPLE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_LT_PURPLE                                                                                                                                  \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_LT_PURPLE,                                                                                                               \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_purple})
#else
# define UWVM_COLOR_U8_LT_PURPLE UWVM_AES_U8_LT_PURPLE
#endif

#pragma push_macro("UWVM_COLOR_U8_LT_CYAN")
#undef UWVM_COLOR_U8_LT_CYAN
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_LT_CYAN                                                                                                                                    \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_LT_CYAN,                                                                                                                 \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_lt_cyan})
#else
# define UWVM_COLOR_U8_LT_CYAN UWVM_AES_U8_LT_CYAN
#endif

#pragma push_macro("UWVM_COLOR_U8_WHITE")
#undef UWVM_COLOR_U8_WHITE
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_WHITE                                                                                                                                   \
     ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b,                                                                                               \
                          UWVM_AES_U8_WHITE,                                                                                                                   \
                          ::utils::ansies::win32_text_attr{::utils::log_output.native_handle(), ::utils::ansies::text_attr::foreground_white})
#else
# define UWVM_COLOR_U8_WHITE UWVM_AES_U8_WHITE
#endif

#pragma push_macro("UWVM_COLOR_U8_RGB")
#undef UWVM_COLOR_U8_RGB
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
# define UWVM_COLOR_U8_RGB(r, g, b) ::fast_io::mnp::cond(::utils::ansies::log_win32_use_ansi_b, UWVM_AES_U8_RGB(r, g, b))
#else
# define UWVM_COLOR_U8_RGB(r, g, b) UWVM_AES_U8_RGB(r, g, b)
#endif
