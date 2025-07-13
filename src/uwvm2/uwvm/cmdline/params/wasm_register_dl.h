/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-31
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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.utils.cmdline;
#else
// std
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/utils/cmdline/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::cmdline::params
{
#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))

    namespace details
    {
        inline constexpr ::fast_io::u8string_view wasm_register_dl_alias{u8"-Wrdl"};
        extern "C++" ::uwvm2::utils::cmdline::parameter_return_type wasm_register_dl_callback(::uwvm2::utils::cmdline::parameter_parsing_results*,
                                                                                              ::uwvm2::utils::cmdline::parameter_parsing_results*,
                                                                                              ::uwvm2::utils::cmdline::parameter_parsing_results*) noexcept;

    }  // namespace details

# if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wbraced-scalar-init"
# endif
    inline constexpr ::uwvm2::utils::cmdline::parameter wasm_register_dl{
        .name{u8"--wasm-register-dl"},
        .describe{u8"Loads a dynamic library and registers its exported functions as a WebAssembly module."},
        .usage{u8"<dl> (<rename>)"},
        .alias{::uwvm2::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::wasm_register_dl_alias), 1uz}},
        .handle{::std::addressof(details::wasm_register_dl_callback)},
        .cate{::uwvm2::utils::cmdline::categorization::wasm}};
# if defined(__clang__)
#  pragma clang diagnostic pop
# endif

#endif
}  // namespace uwvm2::uwvm::cmdline::params

#ifndef UWVM_MODULE
// macro
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif
