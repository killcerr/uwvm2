﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

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

#pragma once

#ifdef UWVM_MODULE
import fast_io;
import ulte.utils.cmdline;
#else
// std
# include <memory>
// macro
# include <ulte/utils/macro/push_macros.h>
# include <ulte/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <ulte/utils/cmdline/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace ulte::uwvm::cmdline::paras
{
    namespace details
    {
        inline bool help_is_exist{};
        inline constexpr ::fast_io::u8string_view help_alias{u8"-h"};
        extern "C++" ::ulte::utils::cmdline::parameter_return_type help_callback(::ulte::utils::cmdline::parameter_parsing_results*,
                                                                                 ::ulte::utils::cmdline::parameter_parsing_results*,
                                                                                 ::ulte::utils::cmdline::parameter_parsing_results*) noexcept;

    }  // namespace details

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif
    inline constexpr ::ulte::utils::cmdline::parameter help{.name{u8"--help"},
                                                            .describe{u8"Get help information."},
                                                            .usage{u8"[<null>|all|global|debug|wasm]"},
                                                            .alias{::ulte::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::help_alias), 1}},
                                                            .handle{::std::addressof(details::help_callback)},
                                                            .is_exist{::std::addressof(details::help_is_exist)}};
#if defined(__clang__)
# pragma clang diagnostic pop
#endif
}  // namespace ulte::uwvm::cmdline::paras

#ifndef UWVM_MODULE
// macro
# include <ulte/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <ulte/utils/macro/pop_macros.h>
#endif
