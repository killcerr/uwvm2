/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-31
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

export module uwvm.cmdline.params:wasm_binfmt;

import fast_io;
import utils.io;
import utils.cmdline;

export namespace uwvm::cmdline::paras
{
    namespace details
    {
        inline bool wasm_binfmt_is_exist{};
        inline constexpr ::fast_io::u8string_view wasm_binfmt_alias{u8"-Wbf"};
        extern "C++" ::utils::cmdline::parameter_return_type wasm_binfmt_callback(::utils::cmdline::parameter_parsing_results*,
                                                                                  ::utils::cmdline::parameter_parsing_results*,
                                                                                  ::utils::cmdline::parameter_parsing_results*) noexcept;

    }  // namespace details

    inline constexpr ::utils::cmdline::parameter wasm_binfmt{.name{u8"--wasm-binfmt"},
                                                             .describe{u8"Specifies the Binary Format Version used by the WASM module, (DEFAULT: 1)."},
                                                             .usage{u8"[1]"},
                                                             .alias{::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::wasm_binfmt_alias), 1}},
                                                             .handle{::std::addressof(details::wasm_binfmt_callback)},
                                                             .is_exist{::std::addressof(details::wasm_binfmt_is_exist)},
                                                             .cate{::utils::cmdline::categorization::wasm}};
}  // namespace uwvm::cmdline::paras
