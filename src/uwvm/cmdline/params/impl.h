/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-27
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

#ifdef UWVM_MODULE
export module uwvm.cmdline.params;
export import :version;
export import :run;
export import :help;
export import :mode;
export import :wasm_abi;
export import :wasm_binfmt;

# ifdef _DEBUG
export import :test;
# endif
#else
# include "version.h"
# include "run.h"
# include "help.h"
# include "mode.h"
# include "wasm_abi.h"
# include "wasm_binfmt.h"
# ifdef _DEBUG
#  include "test.h"
# endif
#endif
