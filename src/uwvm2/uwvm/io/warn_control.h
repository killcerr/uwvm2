/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @file        io_device.cppm
 * @brief       Set up the lowest-level implementation of uwvm.
 *              Use the C API in the minimal C host environment and the user-space lowest-level API in other environments.
 *              Automatically add file-form buffers to out and in.
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-21
 * @copyright   ASHP-1.0 License
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

#ifdef UWVM_MODULE
// import
import fast_io;
#else
// import
# include <fast_io.h>
# include <fast_io_device.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace uwvm2::uwvm
{
    inline bool show_warning{true};  // [global] No global variable dependencies from other translation units
}  // namespace uwvm2::utils
