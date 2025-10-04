/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-10-01
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
// std
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/cmdline/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::cmdline::params
{
    namespace details
    {
        inline constexpr ::uwvm2::utils::container::u8string_view wasi_mount_root_alias{u8"-Iroot"};
        extern "C++" void wasi_mount_root_pretreatment(char8_t const* const*& argv_curr,
                                                       char8_t const* const* argv_end,
                                                       ::uwvm2::utils::container::vector<::uwvm2::utils::cmdline::parameter_parsing_results>& pr) noexcept;
        extern "C++" ::uwvm2::utils::cmdline::parameter_return_type wasi_mount_root_callback(::uwvm2::utils::cmdline::parameter_parsing_results*,
                                                                                             ::uwvm2::utils::cmdline::parameter_parsing_results*,
                                                                                             ::uwvm2::utils::cmdline::parameter_parsing_results*) noexcept;

    }  // namespace details

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif
    inline constexpr ::uwvm2::utils::cmdline::parameter wasi_mount_root{
        .name{u8"--wasi-mount-root"},
        .describe{
            u8"Mount a host directory to the WASI sandbox. Only the root directory existence is validated at parse time. At runtime, wildcard automata determine access with precedence: symlink-escape-nonwasi (symlink-only, highest), WASI whitelist (runtime-managed, not handled here), whitelist (-add), blacklist (-rm), then normal files. Patterns support *, ?, ** and {a,b}. WASI cannot create symlinks under symlink-escape-nonwasi matched paths but may create regular files there."},
        .usage{u8"<dir:str> (-add <pattern>...) (-rm <pattern>...) (--symlink-escape-nonwasi <pattern>...)"},
        .alias{::uwvm2::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::wasi_mount_root_alias), 1uz}},
        .handle{::std::addressof(details::wasi_mount_root_callback)},
        .pretreatment{::std::addressof(details::wasi_mount_root_pretreatment)},
        .cate{::uwvm2::utils::cmdline::categorization::wasi}};
#if defined(__clang__)
# pragma clang diagnostic pop
#endif
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif

