/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

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
import ulte.parser.wasm.base;
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.parser.wasm.standard.wasm1.opcode;
import ulte.parser.wasm.binfmt.binfmt_ver1;
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
# include <ulte/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <ulte/parser/wasm/base/impl.h>
# include <ulte/parser/wasm/concepts/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
# include <ulte/parser/wasm/standard/wasm1/section/impl.h>
# include <ulte/parser/wasm/standard/wasm1/opcode/impl.h>
# include <ulte/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::features
{
    ///
    /// @brief Defining structures or concepts related to wasm1 versions
    ///

    ///////////////////////////
    /// @brief type section ///
    ///////////////////////////

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct type_section_storage_t;

    /// @brief Define functions for checking value_type to provide extensibility
    template <typename... Fs>
    concept has_check_value_type = requires(::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                            ::ulte::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> value_type) {
        { define_check_value_type(sec_adl, value_type) } -> ::std::same_as<bool>;
    };

    /// @brief Define functions to handle type prefix
    template <typename... Fs>
    concept has_type_prefix_handler = requires(::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                               ::ulte::parser::wasm::standard::wasm1::features::final_type_prefix_t<Fs...> preifx,
                                               ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* section_curr,
                                               ::std::byte const* const section_end,
                                               ::ulte::parser::wasm::base::error_impl& err,
                                               ::std::byte const* const prefix_module_ptr) {
        {
            define_type_prefix_handler(sec_adl, preifx, module_storage, section_curr, section_end, err, prefix_module_ptr)
        } -> ::std::same_as<::std::byte const*>;
    };

    /////////////////////////////
    /// @brief import section ///
    /////////////////////////////

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t;

    /// @brief Define functions for handle extern_prefix
    template <typename... Fs>
    concept has_extern_prefix_imports_handler =
        requires(::ulte::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                 ::ulte::parser::wasm::standard::wasm1::features::final_import_type<Fs...>& fit,
                 ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::ulte::parser::wasm::base::error_impl& err) {
            {
                define_extern_prefix_imports_handler(sec_adl, fit.imports, module_storage, section_curr, section_end, err)
            } -> ::std::same_as<::std::byte const*>;
        };

    /// @brief Defining structures or concepts related to wasm versions
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct wasm1_final_extern_type
    {
        union storage_t
        {
            ::ulte::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const* function;
            ::ulte::parser::wasm::standard::wasm1::type::table_type table;    /// @todo
            ::ulte::parser::wasm::standard::wasm1::type::memory_type memory;  /// @todo
            ::ulte::parser::wasm::standard::wasm1::type::global_type global;  /// @todo
        } storage;

        ::ulte::parser::wasm::standard::wasm1::type::external_types type{};
    };

}  // namespace ulte::parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <ulte/utils/macro/pop_macros.h>
#endif
