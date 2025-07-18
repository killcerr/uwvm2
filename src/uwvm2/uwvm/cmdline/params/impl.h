/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-27
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
// global
# include "version.h"
# include "run.h"
# include "help.h"
# include "mode.h"

// debug
# ifdef _DEBUG
#  include "debug_test.h"
# endif

// wasm
# include "wasm_set_main_module_name.h"
# include "wasm_preload_library.h"
# include "wasm_register_dl.h"

// log
# include "log_output.h"
# include "log_disable_warning.h"
# include "log_verbose.h"
# include "log_win32_use_ansi.h"

#endif
