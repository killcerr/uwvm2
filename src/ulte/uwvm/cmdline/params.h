﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

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
import ulte.utils.cmdline;
import ulte.uwvm.cmdline.params;
#else
// std
# include <memory>
// import
# include <ulte/utils/cmdline/impl.h>
# include "params/impl.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::uwvm::cmdline
{
    namespace details
    {
        inline constexpr ::ulte::utils::cmdline::parameter const* parameter_unsort[]{
            ::std::addressof(::ulte::uwvm::cmdline::paras::version),
            ::std::addressof(::ulte::uwvm::cmdline::paras::run),
            ::std::addressof(::ulte::uwvm::cmdline::paras::help),
            ::std::addressof(::ulte::uwvm::cmdline::paras::mode),
            ::std::addressof(::ulte::uwvm::cmdline::paras::wasm_abi),
            ::std::addressof(::ulte::uwvm::cmdline::paras::wasm_binfmt),
            ::std::addressof(::ulte::uwvm::cmdline::paras::u8log_output),

#ifdef _DEBUG
            ::std::addressof(::ulte::uwvm::cmdline::paras::test),
#endif

#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
            ::std::addressof(::ulte::uwvm::cmdline::paras::log_win32_use_ansi),
#endif
        };
    }  // namespace details

    inline constexpr auto parameters{::ulte::utils::cmdline::parameter_sort(details::parameter_unsort)};

    inline constexpr ::std::size_t parameter_lookup_table_size{::ulte::utils::cmdline::calculate_alias_parameters_size(parameters)};
    inline constexpr auto parameter_lookup_table{::ulte::utils::cmdline::expand_alias_parameters_and_check<parameter_lookup_table_size>(parameters)};

    inline constexpr ::std::size_t parameter_max_principal_name_size{::ulte::utils::cmdline::calculate_max_principal_name_size(parameters)};
    inline constexpr ::std::size_t parameter_max_name_size{::ulte::utils::cmdline::calculate_max_para_size(parameter_lookup_table)};

    inline constexpr auto hash_table_size{::ulte::utils::cmdline::calculate_hash_table_size(parameter_lookup_table)};
    inline constexpr auto hash_table{
        ::ulte::utils::cmdline::generate_hash_table<hash_table_size.hash_table_size, hash_table_size.extra_size, hash_table_size.real_max_conflict_size>(
            parameter_lookup_table)};

    inline constexpr ::std::size_t hash_table_byte_sz{sizeof(hash_table)};
    // inline constexpr auto sizeof_hash_table{sizeof(hash_table)};
}  // namespace ulte::uwvm::cmdline
