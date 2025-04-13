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

module;

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>
#include <algorithm>

#include <utils/macro/push_macros.h>

export module parser.wasm.binfmt.binfmt_ver1:handler;

import fast_io;
import utils.io;
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.binfmt.base;
import :section;
import :def;

export namespace parser::wasm::binfmt::ver1
{
    template <typename Sec, typename... Fs>
    concept has_handle_binfmt_ver1_extensible_section_define =
        requires(::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec>> ref,
                 ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_begin,
                 ::std::byte const* section_end) {
            { handle_binfmt_ver1_extensible_section_define(ref, module_storage, section_begin, section_end) } -> ::std::same_as<bool>;
        };

    /// @see WebAssembly Release 1.0 (2019-07-20) § 5.5.2
    template <typename Ty>
    concept has_section_id_define =
        requires { ::std::same_as<decltype(::std::remove_cvref_t<Ty>::section_id), ::parser::wasm::standard::wasm1::type::wasm_u32>; };

    template <typename Ty, typename... Fs>
    concept has_section_id_and_handle_binfmt_ver1_extensible_section_define =
        has_handle_binfmt_ver1_extensible_section_define<Ty, Fs...> && has_section_id_define<Ty>;

    template <typename ...Ty>
    inline consteval void check_extensible_section_is_series(::fast_io::tuple<Ty...>) noexcept
    {
        ::std::vector<::parser::wasm::standard::wasm1::type::wasm_u32> vec{};
        [&vec]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        { ((vec.push_back(Ty...[I] ::section_id)), ...); }(::std::make_index_sequence<sizeof...(Ty)>{});
        ::std::ranges::sort(vec);
        ::parser::wasm::standard::wasm1::type::wasm_u32 counter{1};
        for(auto i: vec)
        {
            if(i != counter++) { ::fast_io::fast_terminate(); }
        }
    }

    /// @brief      handle all binfmt ver1 extensible section
    /// @throws     ::fast_io::error
    /// @see        test\non-platform-specific\0001.parser\0002.binfmt1\handle_all_binfmt_ver1_extensible_section.cc
    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void
        handle_all_binfmt_ver1_extensible_section(::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                                  ::parser::wasm::standard::wasm1::type::wasm_u32 section_id,
                                                  ::std::byte const* section_begin,
                                                  ::std::byte const* section_end) UWVM_THROWS
    {
        auto [... secs]{module_storage.sections};
        check_extensible_section_is_series(module_storage.sections);
        constexpr auto Secs_sz{sizeof...(secs)};

        bool finish{};

        [&finish, &module_storage, section_id, section_begin, section_end]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            ((
                 [&finish, &module_storage, section_id, section_begin, section_end]<typename SecCurr>() constexpr noexcept
                 {
                     static_assert(has_section_id_and_handle_binfmt_ver1_extensible_section_define<SecCurr, Fs...>);
                     if(section_id == SecCurr::section_id)
                     {
                         if(finish) { return; }
                         finish = handle_binfmt_ver1_extensible_section_define(::parser::wasm::concepts::feature_reserve_type<::std::remove_cvref_t<SecCurr>>,
                                                                               module_storage,
                                                                               section_begin,
                                                                               section_end);
                     }
                 }.template operator()<decltype(secs...[I])>()),
             ...);
        }(::std::make_index_sequence<Secs_sz>{});

        if(!finish) [[unlikely]] { ::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file); }
    }

    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr wasm_binfmt_ver1_module_extensible_storage_t<Fs...>
        wasm_binfmt_ver1_handle_func(::fast_io::tuple<Fs...>, ::std::byte const* module_begin, ::std::byte const* module_end) UWVM_THROWS
    {
        wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};

        ret.module_span.module_begin = module_begin;
        ret.module_span.module_end = module_end;

        /// @todo TODO
        
        return ret;
    }
}  // namespace parser::wasm::binfmt::ver1
