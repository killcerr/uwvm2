/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-26
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
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
import :def;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/standard/wasm1/opcode/impl.h>
# include <parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::standard::wasm1::features
{
    ///
    /// @brief Defining structures or concepts related to wasm1 versions
    ///

    ///////////////////////////
    /// @brief type section ///
    ///////////////////////////

    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct type_section_storage_t;

    /// @brief Define functions for checking value_type to provide extensibility
    template <typename... Fs>
    concept has_check_value_type = requires(::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                            ::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> value_type) {
        { define_check_value_type(sec_adl, value_type) } -> ::std::same_as<bool>;
    };

    /// @brief Define functions to handle type prefix
    template <typename... Fs>
    concept has_type_prefix_handler = requires(::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                               ::parser::wasm::standard::wasm1::features::final_type_prefix_t<Fs...> preifx,
                                               ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* section_curr,
                                               ::std::byte const* const section_end,
                                               ::parser::wasm::base::error_impl& err) {
        { define_type_prefix_handler(sec_adl, preifx, module_storage, section_curr, section_end, err) } -> ::std::same_as<::std::byte const*>;
    };

    /////////////////////////////
    /// @brief import section ///
    /////////////////////////////

    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t;

    /// @brief Define functions for handle extern_prefix
    template <typename... Fs>
    concept has_extern_prefix_imports_handler = requires(::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                                                         ::parser::wasm::standard::wasm1::features::final_import_type<Fs...>& fit,
                                                         ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                                         ::std::byte const* section_curr,
                                                         ::std::byte const* const section_end,
                                                         ::parser::wasm::base::error_impl& err) {
        { define_extern_prefix_imports_handler(sec_adl, fit.imports, module_storage, section_curr, section_end, err) } -> ::std::same_as<::std::byte const*>;
    };

    /// @brief Defining structures or concepts related to wasm versions
    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct wasm1_final_extern_type
    {
        union storage_t
        {
            ::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const* function;
            ::parser::wasm::standard::wasm1::type::table_type table;    /// @todo
            ::parser::wasm::standard::wasm1::type::memory_type memory;  /// @todo
            ::parser::wasm::standard::wasm1::type::global_type global;  /// @todo
        } storage;

        ::parser::wasm::standard::wasm1::type::external_types type{};
    };

}  // namespace parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
