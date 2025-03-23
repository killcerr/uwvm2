/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-23
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

// #pragma once /// pragma once is not necessary

#pragma push_macro("UWVM_AES_U8_BLACK")
#undef UWVM_AES_U8_BLACK
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_BLACK
#else
#define UWVM_AES_U8_BLACK u8"\033[30m"
#endif

#pragma push_macro("UWVM_AES_U8_RED")
#undef UWVM_AES_U8_RED
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_RED
#else
#define UWVM_AES_U8_RED u8"\033[31m"
#endif

#pragma push_macro("UWVM_AES_U8_GREEN")
#undef UWVM_AES_U8_GREEN
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_GREEN
#else
#define UWVM_AES_U8_GREEN u8"\033[32m"
#endif

#pragma push_macro("UWVM_AES_U8_ORANGE")
#undef UWVM_AES_U8_ORANGE
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_ORANGE
#else
#define UWVM_AES_U8_ORANGE u8"\033[33m"
#endif

#pragma push_macro("UWVM_AES_U8_BLUE")
#undef UWVM_AES_U8_BLUE
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_BLUE
#else
#define UWVM_AES_U8_BLUE u8"\033[34m"
#endif

#pragma push_macro("UWVM_AES_U8_PURPLE")
#undef UWVM_AES_U8_PURPLE
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_PURPLE
#else
#define UWVM_AES_U8_PURPLE u8"\033[35m"
#endif

#pragma push_macro("UWVM_AES_U8_CYAN")
#undef UWVM_AES_U8_CYAN
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_CYAN
#else
#define UWVM_AES_U8_CYAN u8"\033[36m"
#endif

#pragma push_macro("UWVM_AES_U8_GRAY")
#undef UWVM_AES_U8_GRAY
#if (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_GRAY
#else
#define UWVM_AES_U8_GRAY u8"\033[37m"
#endif

#pragma push_macro("UWVM_AES_U8_DK_GRAY")
#undef UWVM_AES_U8_DK_GRAY
#if defined(__DJGPP__)
#define UWVM_AES_U8_DK_GRAY UWVM_AES_U8_BLACK
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_DK_GRAY
#else
#define UWVM_AES_U8_DK_GRAY u8"\033[90m"
#endif

#pragma push_macro("UWVM_AES_U8_LT_RED")
#undef UWVM_AES_U8_LT_RED
#if defined(__DJGPP__)
#define UWVM_AES_U8_LT_RED UWVM_AES_U8_RED
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_LT_RED
#else
#define UWVM_AES_U8_LT_RED u8"\033[91m"
#endif

#pragma push_macro("UWVM_AES_U8_LT_GREEN")
#undef UWVM_AES_U8_LT_GREEN
#if defined(__DJGPP__)
#define UWVM_AES_U8_LT_GREEN UWVM_AES_U8_GREEN
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_LT_GREEN
#else
#define UWVM_AES_U8_LT_GREEN u8"\033[92m"
#endif

#pragma push_macro("UWVM_AES_U8_YELLOW")
#undef UWVM_AES_U8_YELLOW
#if defined(__DJGPP__)
#define UWVM_AES_U8_YELLOW UWVM_AES_U8_ORANGE
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_YELLOW
#else
#define UWVM_AES_U8_YELLOW u8"\033[93m"
#endif

#pragma push_macro("UWVM_AES_U8_LT_BLUE")
#undef UWVM_AES_U8_LT_BLUE
#if defined(__DJGPP__)
#define UWVM_AES_U8_LT_BLUE UWVM_AES_U8_BLUE
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_LT_BLUE
#else
#define UWVM_AES_U8_LT_BLUE u8"\033[94m"
#endif

#pragma push_macro("UWVM_AES_U8_LT_PURPLE")
#undef UWVM_AES_U8_LT_PURPLE
#if defined(__DJGPP__)
#define UWVM_AES_U8_LT_PURPLE UWVM_AES_U8_PURPLE
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_LT_PURPLE
#else
#define UWVM_AES_U8_LT_PURPLE u8"\033[95m"
#endif

#pragma push_macro("UWVM_AES_U8_LT_CYAN")
#undef UWVM_AES_U8_LT_CYAN
#if defined(__DJGPP__)
#define UWVM_AES_U8_LT_CYAN UWVM_AES_U8_CYAN
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_LT_CYAN
#else
#define UWVM_AES_U8_LT_CYAN u8"\033[96m"
#endif

#pragma push_macro("UWVM_AES_U8_WHITE")
#undef UWVM_AES_U8_WHITE
#if defined(__DJGPP__)
#define UWVM_AES_U8_WHITE UWVM_AES_U8_GRAY
#elif (defined(_WIN32) && defined(_WIN32_WINDOWS)) || (defined(__MSDOS__) && !defined(__DJGPP__))
#define UWVM_AES_U8_WHITE
#else
#define UWVM_AES_U8_WHITE u8"\033[97m"
#endif

