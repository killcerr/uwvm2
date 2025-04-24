/********************************************************
 * Ultimate WebAssembly Virtual Machine (help 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-30
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
#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))

# include <memory>

# include <utils/macro/push_macros.h>
# include <utils/ansies/ansi_push_macro.h>

# ifdef UWVM_MODULE
import fast_io;
import utils.io;
import utils.ansies;
import utils.cmdline;
import utils.ansies;
import parser.wasm.base;
import uwvm.cmdline;
# else
#  include <fast_io.h>
#  include <utils/io/impl.h>
#  include <utils/ansies/impl.h>
#  include <utils/cmdline/impl.h>
#  include <utils/ansies/impl.h>
#  include <parser/wasm/base/impl.h>
#  include <uwvm/cmdline/impl.h>
# endif

namespace uwvm::cmdline::paras::details
{
    UWVM_GNU_COLD extern ::utils::cmdline::parameter_return_type log_win32_use_ansi_callback(::utils::cmdline::parameter_parsing_results*,
                                                                                             ::utils::cmdline::parameter_parsing_results*,
                                                                                             ::utils::cmdline::parameter_parsing_results*) noexcept
    {
        ::utils::ansies::log_win32_use_ansi_b = true;
        return ::utils::cmdline::parameter_return_type::def;
    }

}  // namespace uwvm::cmdline::paras::details

// macro
# include <utils/ansies/ansi_pop_macro.h>
# include <utils/macro/pop_macros.h>

#endif
