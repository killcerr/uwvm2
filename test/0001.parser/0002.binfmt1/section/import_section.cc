/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-06-15
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

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <concepts>
#include <memory>

#include <uwvm2/utils/macro/push_macros.h>

#ifndef UWVM_MODULE
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
#else
# error "Module testing is not currently supported"
#endif

enum class vt1 : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
{

};

enum class vt2 : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
{

};

enum class vt3 : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
{

};

enum class vt4 : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
{

};

enum class vt5 : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
{

};

struct F1
{
    inline static constexpr ::uwvm2::utils::container::u8string_view feature_name{u8"F1"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    using global_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
    using table_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
    using memory_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
};

struct F2
{
    inline static constexpr ::uwvm2::utils::container::u8string_view feature_name{u8"F2"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    using global_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt3>;
    using table_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt4>;
    using memory_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt5>;
};

// Feature with controllable check_duplicate_imports parameter
struct F3
{
    inline static constexpr ::uwvm2::utils::container::u8string_view feature_name{u8"F3"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    // Define parameter struct for controllable check_duplicate_imports
    struct parameter
    {
        bool controllable_check_duplicate_imports{false};
    };
};

// Feature without controllable check_duplicate_imports parameter
struct F4
{
    inline static constexpr ::uwvm2::utils::container::u8string_view feature_name{u8"F4"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    // No parameter struct defined
};

// Feature with controllable check_duplicate_imports parameter set to true
struct F5
{
    inline static constexpr ::uwvm2::utils::container::u8string_view feature_name{u8"F5"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    // Define parameter struct for controllable check_duplicate_imports
    struct parameter
    {
        bool controllable_check_duplicate_imports{true};
    };
};

int main()
{
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<F1>, vt1>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<F1>, vt1>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<F1>, vt1>);

    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<F1, F2>, vt3>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<F1, F2>, vt4>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<F1, F2>, vt5>);

    // Test has_feature_parameter_controllable_check_duplicate_imports concept
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F3>);
    static_assert(!::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F4>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F5>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F1, F3>);
    static_assert(!::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F1, F4>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<F1, F5>);

    // Test runtime get_feature_parameter_controllable_check_duplicate_imports_from_paras
    ::uwvm2::parser::wasm::concepts::feature_parameter_t<F3> para_F3{};
    auto const test_controllable_check_duplicate_imports_F3{
        ::uwvm2::parser::wasm::standard::wasm1::features::get_feature_parameter_controllable_check_duplicate_imports_from_paras(para_F3)};
    if(test_controllable_check_duplicate_imports_F3 != false)
    {
        ::fast_io::print("test_controllable_check_duplicate_imports_F3 is not false\n");
        ::fast_io::fast_terminate();
    }

    ::uwvm2::parser::wasm::concepts::feature_parameter_t<F5> para_F5{};
    auto const test_controllable_check_duplicate_imports_F5{
        ::uwvm2::parser::wasm::standard::wasm1::features::get_feature_parameter_controllable_check_duplicate_imports_from_paras(para_F5)};
    if(test_controllable_check_duplicate_imports_F5 != true)
    {
        ::fast_io::print("test_controllable_check_duplicate_imports_F5 is not true\n");
        ::fast_io::fast_terminate();
    }

    ::uwvm2::parser::wasm::concepts::feature_parameter_t<F1, F3> para_F1F3{};
    auto const test_controllable_check_duplicate_imports_F1F3{
        ::uwvm2::parser::wasm::standard::wasm1::features::get_feature_parameter_controllable_check_duplicate_imports_from_paras(para_F1F3)};
    if(test_controllable_check_duplicate_imports_F1F3 != false)
    {
        ::fast_io::print("test_controllable_check_duplicate_imports_F1F3 is not false\n");
        ::fast_io::fast_terminate();
    }

    ::uwvm2::parser::wasm::concepts::feature_parameter_t<F1, F5> para_F1F5{};
    auto const test_controllable_check_duplicate_imports_F1F5{
        ::uwvm2::parser::wasm::standard::wasm1::features::get_feature_parameter_controllable_check_duplicate_imports_from_paras(para_F1F5)};
    if(test_controllable_check_duplicate_imports_F1F5 != true)
    {
        ::fast_io::print("test_controllable_check_duplicate_imports_F1F5 is not true\n");
        ::fast_io::fast_terminate();
    }
}

// macro
#include <uwvm2/utils/macro/pop_macros.h>
