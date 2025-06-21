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
#else
// std
# include <cstddef>
# include <cstdint>
# include <stdfloat>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::base
{
    /// @brief define possible errors in wasm parser
    enum class wasm_parse_error_code : ::std::uint_least32_t
    {
        ok = 0u,
        illegal_begin_pointer,
        illegal_wasm_file_format,
        no_wasm_section_found,
        invalid_section_length,
        illegal_section_length,
        not_enough_space_unmarked,
        illegal_section_id,
        invalid_custom_name_length,
        illegal_custom_name_length,
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
        import_missing_import_type,
        illegal_importdesc_prefix,
        invalid_type_index,
        illegal_type_index,
        not_enough_space,
        table_type_cannot_find_element,
        table_type_illegal_element,
        limit_type_cannot_find_flag,
        limit_type_illegal_flag,
        limit_type_invalid_min,
        limit_type_invalid_max,
        global_type_cannot_find_valtype,
        global_type_illegal_valtype,
        global_type_cannot_find_mut,
        global_type_illegal_mut,
        invalid_func_count,
        func_section_resolved_exceeded_the_actual_number,
        func_section_resolved_not_match_the_actual_number,
        size_exceeds_the_maximum_value_of_size_t,
        duplicate_imports_of_the_same_import_type,
        invalid_utf8_sequence
    };

    /// @brief used for duplicate_imports_of_the_same_import_type and duplicate_exports_of_the_same_export_type
    struct duplicate_imports_or_exports_t
    {
        ::fast_io::u8string_view module_name;
        ::fast_io::u8string_view extern_name;
        ::std::uint_least8_t type;
    };

    /// @brief define IEEE 754 F32 and F64
#ifdef __STDCPP_FLOAT32_T__
    using error_f32 = ::std::float32_t;  // IEEE 754-2008
#else
    using error_f32 = float;  // The C++ Standard doesn't specify it. Gotta check.
#endif
    static_assert(::std::numeric_limits<error_f32>::is_iec559 && ::std::numeric_limits<error_f32>::digits == 24 &&
                      ::std::numeric_limits<error_f32>::max_exponent == 128 && ::std::numeric_limits<error_f32>::min_exponent == -125,
                  "error_f32 ain't of the IEC 559/IEEE 754 floating-point types");

#ifdef __STDCPP_FLOAT64_T__
    using error_f64 = ::std::float64_t;  // IEEE 754-2008
#else
    using error_f64 = double;  // The C++ Standard doesn't specify it. Gotta check.
#endif
    static_assert(::std::numeric_limits<error_f64>::is_iec559 && ::std::numeric_limits<error_f64>::digits == 53 &&
                      ::std::numeric_limits<error_f64>::max_exponent == 1024 && ::std::numeric_limits<error_f64>::min_exponent == -1021,
                  "error_f64 ain't of the IEC 559/IEEE 754 floating-point types");

    /// @brief Additional information provided by wasm error
    union err_selectable_t
    {
        duplicate_imports_or_exports_t duplic_imports_or_exports;

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

        error_f64 f64;
        error_f32 f32;
        bool boolean;

        ::std::uint_least64_t u64arr[1];
        ::std::int_least64_t i64arr[1];
        ::std::uint_least32_t u32arr[2];
        ::std::int_least32_t i32arr[2];
        ::std::uint_least16_t u16arr[4];
        ::std::int_least16_t i16arr[4];
        ::std::uint_least8_t u8arr[8];
        ::std::int_least8_t i8arr[8];

        error_f64 f64arr[1];
        error_f32 f32arr[2];
        bool booleanarr[8];
    };

    /// @brief Structured structure, with reference form as formal parameter
    struct error_impl
    {
        err_selectable_t err_selectable{};
        ::std::byte const* err_curr{};
        wasm_parse_error_code err_code{};
    };

}  // namespace uwvm2::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
