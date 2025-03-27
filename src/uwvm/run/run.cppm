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

module;

#include <utils/ansies/ansi_push_macro.h>

export module uwvm.run:run;

import fast_io;
import utils.io;
import uwvm.cmdline;

export namespace uwvm::run
{
    inline int run() noexcept
    {
        if(!::uwvm::cmdline::wasm_file_ppos) [[unlikely]]
        {
            ::fast_io::io::perr(
                ::utils::u8err,
                UWVM_AES_U8_RST_ALL
                    UWVM_AES_U8_WHITE u8"uwvm: " UWVM_AES_U8_RED u8"[error] " UWVM_AES_U8_WHITE u8"No input file.\n\n" UWVM_AES_U8_RST_ALL);
            return -1;
        }

        return 0;
    }
}  // namespace uwvm::run
