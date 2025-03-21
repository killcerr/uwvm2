/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author MacroModel
 * @version 2.0.0
 * @date 2025-03-21
 * @copyright APL-2 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

/// @brief uwvm.crtmain:global module declaration
export module uwvm.crtmain:global;

/// @brief import utils.global
import utils.global;

/// @brief For msvc, set the initialization order of this global value to user
#    if !__has_cpp_attribute(__gnu__::__init_priority__)
#pragma init_seg("user")
#    endif

export namespace uwvm::global
{
    // This declares the rtti global variable, set __init_priority__ to 400

    /// @brief Automatically setup windows consolecp with ansi
#if (defined(_WIN32) && !defined(__CYGWIN__)) && !defined(_WIN32_WINDOWS)
#    if !defined(_WIN32_WINDOWS) && false /* TO DO*/

// TODO inline ::uwvm::global::set_nt_console_io_cp_to_utf8 set_native_console_io_cp_to_utf8_ele{};

#        if __has_cpp_attribute(__gnu__::__init_priority__)
    [[__gnu__::__init_priority__(400)]]
#        endif
    inline ::uwvm::global::enable_nt_ansi enable_native_ansi_ele{};

#    else

#        if __has_cpp_attribute(__gnu__::__init_priority__)
    [[__gnu__::__init_priority__(400)]]
#        endif
    inline ::uwvm::global::set_win32_console_io_cp_to_utf8 set_native_console_io_cp_to_utf8_ele{};

#        if __has_cpp_attribute(__gnu__::__init_priority__)
    [[__gnu__::__init_priority__(400)]]
#        endif
    inline ::uwvm::global::enable_win32_ansi enable_native_ansi_ele{};

#    endif
#endif

/// @brief Automatically get the correct timezone data
#if __has_cpp_attribute(__gnu__::__init_priority__)
    [[__gnu__::__init_priority__(400)]]
#endif
    inline ::uwvm::global::tz_set_s tz_set_ele{};
}  // namespace uwvm::global
