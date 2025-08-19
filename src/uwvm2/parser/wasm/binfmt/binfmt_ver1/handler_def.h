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

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <cstring>
# include <climits>
# include <concepts>
# include <type_traits>
# include <utility>
# include <vector>
# include <algorithm>
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/utf/impl.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/parser/wasm/text_format/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/binfmt/base/impl.h>
# include "section.h"
# include "def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::binfmt::ver1
{
    // size of char8_t is equal to the size of ::std::byte.

    template <typename Sec, typename... Fs>
    concept has_handle_binfmt_ver1_extensible_section_define =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec>> ref,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* const section_begin,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
                 ::std::byte const* const sec_id_module_ptr) {
            {
                handle_binfmt_ver1_extensible_section_define(ref, module_storage, section_begin, section_end, err, fs_para, sec_id_module_ptr)
            } -> ::std::same_as<void>;
        };

    /// @see WebAssembly Release 1.0 (2019-07-20) § 5.5.2
    template <typename Ty>
    concept has_section_id_define =
        requires { requires std::same_as<std::remove_cvref_t<decltype(Ty::section_id)>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>; };

    template <typename Ty>
    concept has_section_name_define =
        requires { requires std::same_as<std::remove_cvref_t<decltype(Ty::section_name)>, ::uwvm2::utils::container::u8string_view>; };

    template <typename Ty, typename... Fs>
    concept has_section_id_and_handle_binfmt_ver1_extensible_section_define =
        has_handle_binfmt_ver1_extensible_section_define<Ty, Fs...> && has_section_id_define<Ty>;

    /// @brief      has final_check type
    /// @details
    ///             ```cpp
    ///
    ///             struct basic_final_check
    ///             {
    ///             };
    ///
    ///             struct F
    ///             {
    ///                 template <typename ... Fs>
    ///                 using final_check = type_replacer<root_of_replacement, basic_final_check>;
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_final_check = requires {
        typename FeatureType::final_check;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::final_check>;
    };

    template <typename FeatureType>
    inline consteval auto get_final_check() noexcept
    {
        if constexpr(has_final_check<FeatureType>) { return typename FeatureType::final_check{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_final_check_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_final_check<Fs>())...>;

    template <typename... Fs>
    concept has_final_check_handler = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<final_final_check_t<Fs...>> final_adl,
                                               ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* const module_end,
                                               ::uwvm2::parser::wasm::base::error_impl& err,
                                               ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        { define_final_check(final_adl, module_storage, module_end, err, fs_para) } -> ::std::same_as<void>;
    };

    namespace details
    {
        template <typename... Sec>
        inline consteval void check_extensible_section_is_series() noexcept
        {
            ::std::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte> vec{};
            [&vec]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
            { ((vec.push_back(Sec...[I] ::section_id)), ...); }(::std::make_index_sequence<sizeof...(Sec)>{});
            ::std::ranges::sort(vec);
            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte counter{0u};
            for(auto i: vec)
            {
#if __cpp_contracts >= 202502L
                contract_assert(counter != ::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>::max());
                contract_assert(i == counter++);
#else
                if(counter == ::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>::max()) { ::fast_io::fast_terminate(); }
                if(i != counter++) { ::fast_io::fast_terminate(); }
#endif
            }
        }

        template <typename... Sec>
        inline consteval ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte generate_section_max_id() noexcept
        {
            static_assert(sizeof...(Sec) != 0);
            ::std::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte> vec{};
            [&vec]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
            { ((vec.push_back(Sec...[I] ::section_id)), ...); }(::std::make_index_sequence<sizeof...(Sec)>{});
            ::std::ranges::sort(vec);
            auto const max{vec.back()};
            return max;
        }

        template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        inline consteval double generate_section_probability_from_Fs() noexcept
        {
#if __cpp_structured_bindings >= 202411L
            // for __builtin_expect_with_probability
            ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};
            auto const& [... secs]{ret.sections};
            constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte max_id{generate_section_max_id<::std::remove_cvref_t<decltype(secs)>...>()};
            constexpr double max_id_probability{1.0 - 1.0 / static_cast<double>(max_id + 1)};
            static_assert(0.0 <= max_id_probability && max_id_probability <= 1.0);
            return max_id_probability;
#else
            // for __builtin_expect_with_probability
            ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};
            constexpr auto get_max_id_from_tuple{[]<typename... Secs> UWVM_ALWAYS_INLINE(::uwvm2::utils::container::tuple<Secs...> const&) constexpr noexcept
                                                 { return generate_section_max_id<Secs...>(); }};
            constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte max_id{get_max_id_from_tuple(ret.sections)};
            constexpr double max_id_probability{1.0 - 1.0 / static_cast<double>(max_id + 1)};
            static_assert(0.0 <= max_id_probability && max_id_probability <= 1.0);
            return max_id_probability;
#endif
        }

        template <typename... Sec>
        inline consteval auto generate_section_name_table() noexcept
        {
            static_assert(sizeof...(Sec) != 0);

            constexpr auto max{generate_section_max_id<Sec...>};
            ::uwvm2::utils::container::array<::uwvm2::utils::container::u8string_view, max + 1u> res{};

            [&res]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
            { ((res[Sec...[I] ::section_id] = Sec...[I] ::section_name), ...); }(::std::make_index_sequence<sizeof...(Sec)>{});

            return res;
        }
    }  // namespace details

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr double probability_from_Fs{details::generate_section_probability_from_Fs<Fs...>()};

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval void check_extensible_section_is_series_from_Fs() noexcept
    {
#if __cpp_structured_bindings >= 202411L
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};
        auto const& [... secs]{ret.sections};
        details::check_extensible_section_is_series<::std::remove_cvref_t<decltype(secs)>...>();
#else
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};
        []<typename... Secs> UWVM_ALWAYS_INLINE(::uwvm2::utils::container::tuple<Secs...> const&) constexpr noexcept -> void
        { details::check_extensible_section_is_series<Secs...>(); }(ret.sections);
#endif
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
