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
 * @date        2025-04-12
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

module;

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>

#include <utils/macro/push_macros.h>

export module parser.wasm.binfmt.binfmt_ver1:section;

import fast_io;
import utils.io;
import parser.wasm.concepts;

export namespace parser::wasm::standard::wasm1::binfmt
{
    template <typename Feature>
    concept has_binfmt_ver1_section_define = ::fast_io::is_tuple<typename Feature::binfmt_ver1_section_type>;

    template <::parser::wasm::concepts::wasm_feature... Features>
    inline consteval auto splice_section_storage_structure() noexcept
    {
        return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            return ((
                        []<typename FeatureCurr>() constexpr noexcept
                        {
                            if constexpr(has_binfmt_ver1_section_define<FeatureCurr>)
                            {
                                return ::parser::wasm::concepts::operation::get_tuple_megger_from_tuple(typename FeatureCurr::binfmt_ver1_section_type{});
                            }
                            else
                            {
                                static_assert(
                                    !(requires { typename FeatureCurr::binfmt_ver1_section_type; } && !has_binfmt_ver1_section_define<FeatureCurr>),
                                    "binfmt_ver1_section_type is not tuple");
                                return ::parser::wasm::concepts::operation::tuple_megger<>{};
                            }
                        }.template operator()<Features...[I]>()),
                    ...);
        }(::std::make_index_sequence<sizeof...(Features)>{});
    }

    template <::parser::wasm::concepts::wasm_feature... Features>
    inline consteval auto splice_section_storage_structure_from_tuple(::fast_io::tuple<Features...>) noexcept
    {
        return splice_section_storage_structure<Features...>();
    }
}  // namespace parser::wasm::standard::wasm1::binfmt
