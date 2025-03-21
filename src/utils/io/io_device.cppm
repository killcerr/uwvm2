/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @file io_device.cppm
 * @brief Set up the lowest-level implementation of uwvm.
 * Use the C API in the minimal C host environment and the user-space lowest-level API in other environments.
 * Automatically add file-form buffers to out and in.
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

/// @brief This is a cpp module
module;

/// @brief utils:io:io_device module declaration
export module utils:io:io_device;

/// @brief import fast_io module
import fast_io;

export namespace uwvm
{
#ifndef __AVR__
    /*
        native stdout:
        on non-windows systems (fd): (int)[0, 1, 2]
        on win9x-win32 (handle): GetStdHandle([STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE])
        on nt (handle): RtlGetCurrentPeb()->ProcessParameters->Standard[input, Output, Error]
     */

    inline ::fast_io::u8native_io_observer u8in{::fast_io::u8in()};
    inline ::fast_io::basic_ibuf<::fast_io::u8native_io_observer> u8in_buf{u8in};
    inline ::fast_io::u8native_io_observer u8out{::fast_io::u8out()};
    inline ::fast_io::basic_obuf<::fast_io::u8native_io_observer> u8out_buf{u8out};
    inline ::fast_io::u8native_io_observer u8err{::fast_io::u8err()};
    // No buffer is provided to err
#else
    // The C API of avrlibc does not have any buffers.

    inline ::fast_io::u8c_io_observer u8in{::fast_io::u8c_stdin()};
    inline ::fast_io::basic_ibuf<::fast_io::u8c_io_observer> u8in_buf{u8in};
    inline ::fast_io::u8c_io_observer u8out{::fast_io::u8c_stdout()};
    inline ::fast_io::basic_obuf<::fast_io::u8c_io_observer> u8ou_buf{u8out};
    inline ::fast_io::u8c_io_observer u8err{::fast_io::u8c_stderr()};
    // No buffer is provided to err
#endif
}  // namespace uwvm
