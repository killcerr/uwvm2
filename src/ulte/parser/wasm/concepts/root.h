﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-06
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
import ulte.parser.wasm.standard.wasm1.type;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <concepts>
// macro
# include <ulte/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/tuple.h>
# include <fast_io_dsal/string_view.h>
# include <ulte/parser/wasm/base/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::concepts
{
    /// @brief Prevent inheritance effects when adl matching
    template <typename FeatureType>
    struct feature_reserve_type_t
    {
        static_assert(::std::is_same_v<::std::remove_cvref_t<FeatureType>, FeatureType>,
                      "feature_reserve_type_t: typename 'FeatureType' cannot have refer and const attributes");
        explicit constexpr feature_reserve_type_t() noexcept = default;
    };

    template <typename FeatureType>
    inline constexpr feature_reserve_type_t<FeatureType> feature_reserve_type{};

    /// @brief For adl function "define_wasm_binfmt_parsering_strategy()" matching
    template <::ulte::parser::wasm::standard::wasm1::type::wasm_u32 Version>
    struct binfmt_version_t
    {
        explicit constexpr binfmt_version_t() noexcept = default;
    };

    template <::ulte::parser::wasm::standard::wasm1::type::wasm_u32 Version>
    inline constexpr binfmt_version_t<Version> binfmt_version{};

    /// @brief      Determine if there is a feature name
    /// @see        test\non-platform-specific\0001.parser\0001.concept\feature_name.cc
    /// @details    Whether the type is provided with a feature name or not, this is a mandatory option.
    ///
    ///             example:
    ///
    ///             Conformity with the concept:
    ///             ```cpp
    ///             struct feature
    ///             {
    ///                 inline static constexpr ::fast_io::u8string_view feature_name{u8"<name>"};
    ///             };
    ///             ```
    ///
    ///             Satisfy the concept, but subsequent operations will be wrong:
    ///             1. Must be a variable that statically stores duration
    ///             ```cpp
    ///             struct feature
    ///             {
    ///                 ::fast_io::u8string_view feature_name{u8"<name>"};
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_feature_name =
        requires { requires ::std::same_as<::std::remove_cvref_t<decltype(::std::remove_cvref_t<FeatureType>::feature_name)>, ::fast_io::u8string_view>; };

    /// @brief      binfmt handle version func
    template <typename module_stroate_t, has_feature_name... Fs>
    using binfmt_handle_version_func_p_type =
        module_stroate_t (*)(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*, ::ulte::parser::wasm::base::error_impl&) UWVM_THROWS;

    /// @brief      Define the version number of the required wasm file binary format tagging
    /// @see        test\non-platform-specific\0001.parser\0001.concept\binfmt.cc
    /// @details    Changing the binfmt version requires a new binfmt version parsing policy, as the entire parsing behavior tree will be replaced with the
    ///             binfmt version. Version must be greater than 0.
    ///
    ///             Whether the type is provided with a binfmt version or not, this is a mandatory option.
    ///
    ///             example:
    ///
    ///             Conformity with the concept:
    ///             ```cpp
    ///             struct feature
    ///             {
    ///                 inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
    ///             };
    ///             ```
    ///
    ///             Satisfy the concept, but subsequent operations will be wrong:
    ///             1. Must be a variable that statically stores duration
    ///             ```cpp
    ///             struct feature
    ///             {
    ///                 ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_wasm_binfmt_version = requires {
        requires ::std::same_as<::std::remove_cvref_t<decltype(::std::remove_cvref_t<FeatureType>::binfmt_version)>,
                                ::ulte::parser::wasm::standard::wasm1::type::wasm_u32>;
    };

    /// @brief      Checks if a "define_wasm_binfmt_parsering_strategy" function is defined
    /// @details    Checks if a version handler function is defined. There can be only one version handler for the same binfmt version
    ///             Cannot define a parsing policy that differs from your wasm version.
    ///
    ///             example:
    ///
    ///             Conformity with the concept:
    ///             ```cpp
    ///             struct feature
    ///             {
    ///                 inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
    ///             };
    ///
    ///             template <wasm_feature ... Fs>
    ///             inline constexpr binfmt_handle_version_func_p_type<Fs...> define_wasm_binfmt_parsering_strategy(feature_reserve_type_t<feature>,
    ///             ::fast_io::tuple<Fs ...>) {}
    ///
    ///             static_assert(has_wasm_binfmt_parsering_strategy<feature>); // OK. Provide a parsing strategy where binfmt version is 1
    ///             ```
    ///
    ///             Doesn't fit the concept:
    ///             1. Version information must be provided
    ///             ```cpp
    ///             struct feature
    ///             {
    ///             };
    ///
    ///             template <wasm_feature ... Fs>
    ///             inline constexpr binfmt_handle_version_func_p_type<Fs...> define_wasm_binfmt_parsering_strategy(feature_reserve_type_t<feature>,
    ///             ::fast_io::tuple<Fs ...>) {}
    ///
    ///             static_assert(has_wasm_binfmt_parsering_strategy<feature>); // ERROR.
    ///             ```
    template <typename FeatureType>
    concept has_wasm_binfmt_parsering_strategy = requires {
        {
            define_wasm_binfmt_parsering_strategy(feature_reserve_type<::std::remove_cvref_t<FeatureType>>, {/* ::fast_io::tuple<Fs ...> */})
        } /* -> binfmt_handle_version_func_p_type<Fs...> */;
    };

    template <typename FeatureType>
    concept wasm_feature = has_feature_name<FeatureType> && has_wasm_binfmt_version<FeatureType> && ::std::is_empty_v<FeatureType>;
}  // namespace ulte::parser::wasm::concepts

#ifndef UWVM_MODULE
// macro
# include <ulte/utils/macro/pop_macros.h>
#endif
