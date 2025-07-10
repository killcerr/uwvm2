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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import uwvm2.uwvm.io;
import uwvm2.uwvm.wasm.storage;
#else
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
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
    inline static constexpr ::fast_io::u8string_view feature_name{u8"F1"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    using global_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
    using table_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
    using memory_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement, vt1>;
};

struct F2
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"F2"};
    inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    using global_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt3>;
    using table_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt4>;
    using memory_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<vt1, vt5>;
};

int main()
{
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<F1>, vt1>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<F1>, vt1>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<F1>, vt1>);

    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<F1, F2>, vt3>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<F1, F2>, vt4>);
    static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<F1, F2>, vt5>);
}

// macro
#include <uwvm2/utils/macro/pop_macros.h>
