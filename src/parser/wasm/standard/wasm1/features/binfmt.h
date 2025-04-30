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
 * @date        2025-04-09
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
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.parser.wasm.standard.wasm1.opcode;
import ulte.parser.wasm.binfmt.binfmt_ver1;
import :def;
import :feature_def;
import :type_section;
import :import_section;
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
# include "feature_def.h"
# include "type_section.h"
# include "import_section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::features
{
    /// @brief wasm1 feature
    struct wasm1
    {
        inline static constexpr ::fast_io::u8string_view feature_name{u8"WebAssembly Release 1.0 (2019-07-20)"};
        inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

        // type section
        using value_type = ::ulte::parser::wasm::concepts::operation::type_replacer<::ulte::parser::wasm::concepts::operation::root_of_replacement,
                                                                              ::ulte::parser::wasm::standard::wasm1::type::value_type>;
        using type_prefix = ::ulte::parser::wasm::concepts::operation::type_replacer<::ulte::parser::wasm::concepts::operation::root_of_replacement,
                                                                               ::ulte::parser::wasm::standard::wasm1::type::function_type_prefix>;

        // import section (template)
        template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
        using extern_type =
            ::ulte::parser::wasm::concepts::operation::type_replacer<::ulte::parser::wasm::concepts::operation::root_of_replacement, wasm1_final_extern_type<Fs...>>;

        // binfmt ver1
        template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
        using binfmt_ver1_section_type = ::fast_io::tuple<::ulte::parser::wasm::standard::wasm1::features::type_section_storage_t<Fs...>,
                                                          ::ulte::parser::wasm::standard::wasm1::features::import_section_storage_t<Fs...>
                                                          /// @todo
                                                          >;
    };

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto define_wasm_binfmt_parsering_strategy(::ulte::parser::wasm::concepts::feature_reserve_type_t<wasm1>, ::fast_io::tuple<Fs...>) noexcept
    {
        return ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_handle_func<Fs...>;
    }

    // feature
    static_assert(::ulte::parser::wasm::concepts::wasm_feature<wasm1>);
    static_assert(::ulte::parser::wasm::concepts::has_wasm_binfmt_parsering_strategy<wasm1>);
    // type section
    static_assert(::ulte::parser::wasm::standard::wasm1::features::has_value_type<wasm1>);
    static_assert(::ulte::parser::wasm::standard::wasm1::features::has_type_prefix<wasm1>);
    // import section
    static_assert(::ulte::parser::wasm::standard::wasm1::features::has_extern_type<wasm1>);
    // binfmt ver1
    static_assert(::ulte::parser::wasm::binfmt::ver1::has_binfmt_ver1_extensible_section_define<wasm1>);
}  // namespace ulte::parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
