/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-27
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
#else
// std
# include <cstddef>
# include <cstdint>
// macro
# include <ulte/utils/macro/push_macros.h>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::base
{
    enum class wasm_parse_error_code : ::std::uint_least32_t
    {
        ok = 0u,
        illegal_wasm_file_format,
        no_wasm_section_found,
        invalid_section_length,
        illegal_section_length,
        no_enough_space,
        illegal_section_id,
        invalid_custom_name_length,
        invalid_parameter_length,
        illegal_parameter_length,
        illegal_value_type,
        invalid_result_length,
        illegal_result_length,
        wasm1_not_allow_multi_value,
        illegal_type_prefix,
        duplicate_section,
        invalid_type_count,
        type_section_resolved_exceeded_the_actual_number,
        type_section_resolved_not_match_the_actual_number,
        forward_dependency_missing,
        invalid_import_count,
        import_section_resolved_exceeded_the_actual_number,
        import_section_resolved_not_match_the_actual_number,
        invalid_import_module_name_length,
        import_module_name_length_cannot_be_zero,
        import_module_name_too_length,
        invalid_import_extern_name_length,
        import_extern_name_length_cannot_be_zero,
        import_extern_name_too_length,
        illegal_importdesc_prefix,
        invalid_type_index,
        illegal_type_index,
    };

    union err_selectable_t
    {
        ::std::byte const* err_end;
        ::std::size_t err_uz;
        ::std::ptrdiff_t err_pdt;

        ::std::uint_least64_t u64;
        ::std::int_least64_t i64;
        ::std::uint_least32_t u32;
        ::std::int_least32_t i32;
        ::std::uint_least16_t u16;
        ::std::int_least16_t i16;
        ::std::uint_least8_t u8;
        ::std::int_least8_t i8;

        double f64;
        float f32;
        bool boolean;

        ::std::uint_least64_t u64arr[1];
        ::std::int_least64_t i64arr[1];
        ::std::uint_least32_t u32arr[2];
        ::std::int_least32_t i32arr[2];
        ::std::uint_least16_t u16arr[4];
        ::std::int_least16_t i16arr[4];
        ::std::uint_least8_t u8arr[8];
        ::std::int_least8_t i8arr[8];

        double f64arr[1];
        float f32arr[2];
        bool booleanarr[8];
    };

    struct error_impl
    {
        err_selectable_t err_selectable{};  // end_selectable can be null
        ::std::byte const* err_curr{};
        wasm_parse_error_code err_code{};
    };

}  // namespace ulte::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <ulte/utils/macro/pop_macros.h>
#endif
