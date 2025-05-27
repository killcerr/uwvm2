/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-27
 * @copyright   ASHP-1.0 License
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
import :error_code;
#else
// std
# include <cstddef>
# include <cstdint>
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
# include "error_code.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::base
{
    /// @brief Define the flag used for output
    struct error_output_flag_t
    {
        ::std::uint_least8_t enable_ansi : 1;
        ::std::uint_least8_t win32_use_text_attr : 1;  // on win95 - win7
    };

    /// @brief Provide information for output
    struct error_output_t
    {
        ::std::byte const* module_begin{};
        ::uwvm2::parser::wasm::base::error_impl err{};
        error_output_flag_t flag{};
    };

    /// @brief Output, support: char, wchar_t, char8_t, char16_t, char32_t
    template <::std::integral char_type, typename Stm>
    inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, error_output_t>, Stm && stream, error_output_t const& errout) noexcept
    {
        bool const enable_ansi{static_cast<bool>(errout.flag.enable_ansi)};
        switch(errout.err.err_code)
        {
            default:
            {
#include "error_code_outputs/eco_default.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::ok:
            {
#include "error_code_outputs/eco_ok.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_begin_pointer:
            {
#include "error_code_outputs/eco_illegal_begin_pointer.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_wasm_file_format:
            {
#include "error_code_outputs/eco_illegal_wasm_file_format.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::no_wasm_section_found:
            {
#include "error_code_outputs/eco_no_wasm_section_found.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_section_length:
            {
#include "error_code_outputs/eco_invalid_section_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_section_length:
            {
#include "error_code_outputs/eco_illegal_section_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::not_enough_space_unmarked:
            {
#include "error_code_outputs/eco_not_enough_space_unmarked.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_section_id:
            {
#include "error_code_outputs/eco_illegal_section_id.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_custom_name_length:
            {
#include "error_code_outputs/eco_invalid_custom_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_custom_name_length:
            {
#include "error_code_outputs/eco_illegal_custom_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_parameter_length:
            {
#include "error_code_outputs/eco_invalid_parameter_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_parameter_length:
            {
#include "error_code_outputs/eco_illegal_parameter_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_value_type:
            {
#include "error_code_outputs/eco_illegal_value_type.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_result_length:
            {
#include "error_code_outputs/eco_invalid_result_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_result_length:
            {
#include "error_code_outputs/eco_illegal_result_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::wasm1_not_allow_multi_value:
            {
#include "error_code_outputs/eco_wasm1_not_allow_multi_value.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_prefix:
            {
#include "error_code_outputs/eco_illegal_type_prefix.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section:
            {
#include "error_code_outputs/eco_duplicate_section.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_count:
            {
#include "error_code_outputs/eco_invalid_type_count.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::type_section_resolved_exceeded_the_actual_number:
            {
#include "error_code_outputs/eco_type_section_resolved_exceeded_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::type_section_resolved_not_match_the_actual_number:
            {
#include "error_code_outputs/eco_type_section_resolved_not_match_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::forward_dependency_missing:
            {
#include "error_code_outputs/eco_forward_dependency_missing.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_import_count:
            {
#include "error_code_outputs/eco_invalid_import_count.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_section_resolved_exceeded_the_actual_number:
            {
#include "error_code_outputs/eco_import_section_resolved_exceeded_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_section_resolved_not_match_the_actual_number:
            {
#include "error_code_outputs/eco_import_section_resolved_not_match_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_import_module_name_length:
            {
#include "error_code_outputs/eco_invalid_import_module_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_module_name_length_cannot_be_zero:
            {
#include "error_code_outputs/eco_import_module_name_length_cannot_be_zero.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_module_name_too_length:
            {
#include "error_code_outputs/eco_import_module_name_too_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_import_extern_name_length:
            {
#include "error_code_outputs/eco_invalid_import_extern_name_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_extern_name_length_cannot_be_zero:
            {
#include "error_code_outputs/eco_import_extern_name_length_cannot_be_zero.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_extern_name_too_length:
            {
#include "error_code_outputs/eco_import_extern_name_too_length.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::import_missing_import_type:
            {
#include "error_code_outputs/eco_import_missing_import_type.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_importdesc_prefix:
            {
#include "error_code_outputs/eco_illegal_importdesc_prefix.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index:
            {
#include "error_code_outputs/eco_invalid_type_index.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index:
            {
#include "error_code_outputs/eco_illegal_type_index.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::not_enough_space:
            {
#include "error_code_outputs/eco_not_enough_space.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::table_type_cannot_find_element:
            {
#include "error_code_outputs/eco_table_type_cannot_find_element.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::table_type_illegal_element:
            {
#include "error_code_outputs/eco_table_type_illegal_element.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_cannot_find_flag:
            {
#include "error_code_outputs/eco_limit_type_cannot_find_flag.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_illegal_flag:
            {
#include "error_code_outputs/eco_limit_type_illegal_flag.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_invalid_min:
            {
#include "error_code_outputs/eco_limit_type_invalid_min.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_invalid_max:
            {
#include "error_code_outputs/eco_limit_type_invalid_max.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_type_cannot_find_valtype:
            {
#include "error_code_outputs/eco_global_type_cannot_find_valtype.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_type_illegal_valtype:
            {
#include "error_code_outputs/eco_global_type_illegal_valtype.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_type_cannot_find_mut:
            {
#include "error_code_outputs/eco_global_type_cannot_find_mut.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_type_illegal_mut:
            {
#include "error_code_outputs/eco_global_type_illegal_mut.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_func_count:
            {
#include "error_code_outputs/eco_invalid_func_count.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number:
            {
#include "error_code_outputs/eco_func_section_resolved_exceeded_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_not_match_the_actual_number:
            {
#include "error_code_outputs/eco_func_section_resolved_not_match_the_actual_number.h"
                return;
            }
            case ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t:
            {
#include "error_code_outputs/eco_size_exceeds_the_maximum_value_of_size_t.h"
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
