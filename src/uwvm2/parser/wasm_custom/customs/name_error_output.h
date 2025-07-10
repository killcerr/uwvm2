/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-27
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
import uwvm2.utils.ansies;
import uwvm2.utils.utf;
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
import uwvm2.utils.debug;
# endif
import :name;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <concepts>
# include <utility>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/utils/ansies/ansi_push_macro.h>
# include <uwvm2/utils/ansies/win32_text_attr_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/ansies/impl.h>
# include <uwvm2/utils/utf/impl.h>
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
#  include <uwvm2/utils/debug/impl.h>
# endif
# include "name.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm_custom::customs
{
    /// @brief Define the flag used for output
    struct name_error_output_flag_t
    {
        ::std::uint_least8_t enable_ansi : 1;
        ::std::uint_least8_t win32_use_text_attr : 1;  // on win95 - win7
    };

    /// @brief Provide information for output
    struct name_error_output_t
    {
        ::std::byte const* name_begin{};
        ::uwvm2::parser::wasm_custom::customs::name_err_t name_err{};
        name_error_output_flag_t flag{};
    };

    /// @brief Output, support: char, wchar_t, char8_t, char16_t, char32_t
    template <::std::integral char_type, typename Stm>
    inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, name_error_output_t>, Stm && stream, name_error_output_t const& errout) noexcept
    {
        bool const enable_ansi{static_cast<bool>(errout.flag.enable_ansi)};
        switch(errout.name_err.type)
        {
            default:
            {
#include "name_error_code_outputs/name_eco_default.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_name_map_length:
            {
#include "name_error_code_outputs/name_eco_invalid_name_map_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::size_exceeds_the_maximum_value_of_size_t:
            {
#include "name_error_code_outputs/name_eco_size_exceeds_the_maximum_value_of_size_t.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_name_map_length:
            {
#include "name_error_code_outputs/name_eco_illegal_name_map_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_module_name_length:
            {
#include "name_error_code_outputs/name_eco_invalid_module_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_module_name_length:
            {
#include "name_error_code_outputs/name_eco_illegal_module_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_utf8_sequence:
            {
#include "name_error_code_outputs/name_eco_illegal_utf8_sequence.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_module_name:
            {
#include "name_error_code_outputs/name_eco_duplicate_module_name.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_function_name:
            {
#include "name_error_code_outputs/name_eco_duplicate_function_name.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_data_exists:
            {
#include "name_error_code_outputs/name_eco_invalid_data_exists.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_name_count:
            {
#include "name_error_code_outputs/name_eco_invalid_function_name_count.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_index:
            {
#include "name_error_code_outputs/name_eco_invalid_function_index.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_name_length:
            {
#include "name_error_code_outputs/name_eco_invalid_function_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_section_id:
            {
#include "name_error_code_outputs/name_eco_illegal_section_id.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_function_name_length:
            {
#include "name_error_code_outputs/name_eco_illegal_function_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_func_idx:
            {
#include "name_error_code_outputs/name_eco_duplicate_func_idx.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_local_name:
            {
#include "name_error_code_outputs/name_eco_duplicate_local_name.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_local_count:
            {
#include "name_error_code_outputs/name_eco_invalid_local_count.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_local_count:
            {
#include "name_error_code_outputs/name_eco_invalid_function_local_count.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_local_index:
            {
#include "name_error_code_outputs/name_eco_invalid_function_local_index.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::invalid_function_local_name_length:
            {
#include "name_error_code_outputs/name_eco_invalid_function_local_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::illegal_function_local_name_length:
            {
#include "name_error_code_outputs/name_eco_illegal_function_local_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_code_function_index:
            {
#include "name_error_code_outputs/name_eco_duplicate_code_function_index.h"
                return;
            }
            case ::uwvm2::parser::wasm_custom::customs::name_err_type_t::duplicate_code_local_name_function_index:
            {
#include "name_error_code_outputs/name_eco_duplicate_code_local_name_function_index.h"
                return;
            }
        }
    }

}  // namespace uwvm2::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/ansies/ansi_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
# include <uwvm2/utils/ansies/win32_text_attr_pop_macro.h>
#endif
