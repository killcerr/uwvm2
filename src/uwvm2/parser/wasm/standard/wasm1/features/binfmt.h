/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-09
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
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.standard.wasm1.section;
import uwvm2.parser.wasm.standard.wasm1.opcode;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import uwvm2.parser.wasm.text_format;
import :def;
import :feature_def;
import :custom_section;
import :type_section;
import :import_section;
import :function_section;
import :table_section;
import :memory_section;
import :global_section;
import :export_section;
import :start_section;
import :element_section;
import :code_section;
import :data_section;
import :final_check;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <uwvm2/parser/wasm/text_format/impl.h>
# include "def.h"
# include "feature_def.h"
# include "custom_section.h"
# include "type_section.h"
# include "import_section.h"
# include "function_section.h"
# include "table_section.h"
# include "memory_section.h"
# include "global_section.h"
# include "export_section.h"
# include "start_section.h"
# include "element_section.h"
# include "code_section.h"
# include "data_section.h"
# include "final_check.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    /// @brief wasm1 feature
    struct wasm1
    {
        inline static constexpr ::fast_io::u8string_view feature_name{u8"WebAssembly Release 1.0 (2019-07-20)"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

        // binary format
        using text_format = ::uwvm2::parser::wasm::concepts::operation::type_replacer<
            ::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
            ::uwvm2::parser::wasm::concepts::text_format_wapper<::uwvm2::parser::wasm::text_format::text_format::utf8_rfc3629_with_zero_illegal>>;

        // type section
        using value_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                     ::uwvm2::parser::wasm::standard::wasm1::type::value_type>;
        using type_prefix = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                      ::uwvm2::parser::wasm::standard::wasm1::type::function_type_prefix>;

        // import section (template)
        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        using extern_type =
            ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                      ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_extern_type<Fs...>>;
        using table_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                     ::uwvm2::parser::wasm::standard::wasm1::type::table_type>;
        using memory_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                      ::uwvm2::parser::wasm::standard::wasm1::type::memory_type>;
        using global_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                      ::uwvm2::parser::wasm::standard::wasm1::type::global_type>;

        // export section
        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        using export_type =
            ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                      ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_export_type<Fs...>>;

        // element section
        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        using element_type =
            ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                      ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>>;
        // data section
        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        using data_type = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                    ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_data_t<Fs...>>;

        // binfmt ver1
        using final_check = ::uwvm2::parser::wasm::concepts::operation::type_replacer<::uwvm2::parser::wasm::concepts::operation::root_of_replacement,
                                                                                      ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_check>;

        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        using binfmt_ver1_section_type = ::fast_io::tuple<::uwvm2::parser::wasm::standard::wasm1::features::custom_section_storage_t,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::type_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::import_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::table_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::memory_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::global_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::export_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::start_section_storage_t,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::element_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::code_section_storage_t<Fs...>,
                                                          ::uwvm2::parser::wasm::standard::wasm1::features::data_section_storage_t<Fs...>>;
    };

    /// @note ADL for distribution to the correct handler function
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto define_wasm_binfmt_parsering_strategy(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<wasm1>,  // [adl]
                                                                ::fast_io::tuple<Fs...>) noexcept
    {
        return ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_handle_func<Fs...>;
    }

    // feature
    static_assert(::uwvm2::parser::wasm::concepts::wasm_feature<wasm1>);
    static_assert(::uwvm2::parser::wasm::concepts::has_wasm_binfmt_parsering_strategy<wasm1>);
    // binary format
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_text_format<wasm1>);
    // type section
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_value_type<wasm1>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_type_prefix<wasm1>);
    // import section
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_extern_type<wasm1>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_table_type<wasm1>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_memory_type<wasm1>);
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_global_type<wasm1>);
    // export section
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_export_type<wasm1>);
    // element setcion
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_element_type<wasm1>);
    // data setcion
    static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_data_type<wasm1>);
    // binfmt ver1
    static_assert(::uwvm2::parser::wasm::binfmt::ver1::has_final_check<wasm1>);
    static_assert(::uwvm2::parser::wasm::binfmt::ver1::has_binfmt_ver1_extensible_section_define<wasm1>);
}  // namespace uwvm2::parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
