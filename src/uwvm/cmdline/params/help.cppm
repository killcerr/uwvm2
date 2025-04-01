﻿/********************************************************
 * Ultimate WebAssembly Virtual Machine (help 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @help     2.0.0
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

module;

#include <memory>

#include <utils/macro/push_macros.h>
#include <utils/ansies/ansi_push_macro.h>

export module uwvm.cmdline.params:help;

import fast_io;
import utils.cmdline;

export namespace uwvm::cmdline::paras
{
    namespace details
    {
        inline bool help_is_exist{};
        inline constexpr ::fast_io::u8string_view help_alias{u8"-h"};
        extern "C++" ::utils::cmdline::parameter_return_type help_callback(::utils::cmdline::parameter_parsing_results*,
                                                                           ::utils::cmdline::parameter_parsing_results*,
                                                                           ::utils::cmdline::parameter_parsing_results*) noexcept;

    }  // namespace details

    inline constexpr ::utils::cmdline::parameter help{.name{u8"--help"},
                                                      .describe{u8"Get help information."},
                                                      .usage{u8"[<null>|global|wasm]"},
                                                      .alias{::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::help_alias), 1}},
                                                      .handle{::std::addressof(details::help_callback)},
                                                      .is_exist{::std::addressof(details::help_is_exist)}};
}  // namespace uwvm::cmdline::paras
