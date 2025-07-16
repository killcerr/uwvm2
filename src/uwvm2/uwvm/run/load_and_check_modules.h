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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.uwvm.io;
import uwvm2.utils.ansies;
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
import uwvm2.utils.debug;
# endif
import uwvm2.utils.madvise;
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard;
import uwvm2.parser.wasm.binfmt.base;
import uwvm2.uwvm.utils.ansies;
import uwvm2.uwvm.utils.memory;
import uwvm2.uwvm.cmdline;
import uwvm2.uwvm.wasm;
import :retval;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <utility>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/utils/ansies/impl.h>
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/utils/madvise/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/impl.h>
# include <uwvm2/parser/wasm/binfmt/base/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/utils/memory/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/wasm/impl.h>
# include "retval.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace uwvm2::uwvm::run
{
    /// @brief Check for duplicates or mismatched imports and exports
    inline int load_and_check_modules() noexcept
    {
        // preload abi
        {
            if(::uwvm2::uwvm::wasm::storage::local_preload_wasip1)
            {
                /// @todo
            }

            if(::uwvm2::uwvm::wasm::storage::local_preload_wasip2)
            {
                /// @todo
            }

            if(::uwvm2::uwvm::wasm::storage::local_preload_wasix)
            {
                /// @todo
            }
        }

        // preloaded wasm
        for(auto const& lwc: ::uwvm2::uwvm::wasm::storage::preloaded_wasm)
        {
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(lwc.module_name)) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    lwc.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                lwc.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(lwc)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm});
        }

#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))

        // preloaded dl
        for(auto const& ldc: ::uwvm2::uwvm::wasm::storage::preloaded_dl)
        {
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(ldc.module_name)) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    ldc.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                ldc.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wd = ::std::addressof(ldc)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl});
        }
#endif

        // exec wasm
        {
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(::uwvm2::uwvm::wasm::storage::execute_wasm.module_name)) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    ::uwvm2::uwvm::wasm::storage::execute_wasm.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                ::uwvm2::uwvm::wasm::storage::execute_wasm.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(::uwvm2::uwvm::wasm::storage::execute_wasm)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm});
        }

        // Checking for import and export inequalities

        for(auto const& curr_module: ::uwvm2::uwvm::wasm::storage::all_module)
        {
            switch(curr_module.second.type)
            {
                case ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm:
                {
                    /// @todo
                    break;
                }
                case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm:
                {
                    /// @todo
                    break;
                }
                case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl:
                {
                    /// @todo
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        return static_cast<int>(::uwvm2::uwvm::run::retval::ok);
    }
}  // namespace uwvm2::uwvm::run

#ifndef UWVM_MODULE
// macro
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif
