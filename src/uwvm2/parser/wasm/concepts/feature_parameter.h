/*************************************************************
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
import uwvm2.parser.wasm.standard.wasm1.type;
import :root;
import :operation;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <concepts>
# include <utility>
# include <algorithm>
# include <vector>
// import
# include <fast_io.h>
# include <fast_io_dsal/tuple.h>
# include <fast_io_dsal/array.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include "root.h"
# include "operation.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::concepts
{
    template <typename Fc>
    concept has_feature_parameter = requires { typename Fc::parameter; };

    /// @brief Splicing the structure used to store feature parameters
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval auto splice_feature_parameter() noexcept
    {
        if constexpr(sizeof...(Fs) == 0) { return ::uwvm2::parser::wasm::concepts::operation::tuple_megger<>{}; }
        else
        {
            return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
            {
                return ((
                            []<::uwvm2::parser::wasm::concepts::wasm_feature Fc>() constexpr noexcept
                            {
                                if constexpr(has_feature_parameter<Fc>)
                                {
                                    using fc_para = typename Fc::parameter;
                                    return ::uwvm2::parser::wasm::concepts::operation::tuple_megger<fc_para>{};
                                }
                                else { return ::uwvm2::parser::wasm::concepts::operation::tuple_megger<>{}; }
                            }.template operator()<Fs...[I]>()),
                        ...);
            }(::std::make_index_sequence<sizeof...(Fs)>{});
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using splice_splice_feature_parameter_t = decltype(splice_feature_parameter<Fs...>())::Type;

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval auto splice_splice_feature_parameter_from_tuple(::fast_io::tuple<Fs...>) noexcept
    {
        return splice_feature_parameter<Fs...>();
    }

    /// @brief      It must be the same as the previous definition (in root.h), so static_assert is used
    /// @details    feature_parameter_t<Fs...> is used to store those parameters used to control feature parsing, such as whether or not to start the parsing
    ///             function, and is passed as a constant reference.
    /// @see        root.h
    template <typename... Fs>
    struct feature_parameter_t
    {
        static_assert((::uwvm2::parser::wasm::concepts::wasm_feature<Fs> && ...), "Fs is not wasm feature");  // check is wasm feature
        using storage_t = splice_splice_feature_parameter_t<Fs...>;                                           // ::fast_io::tuple
        static_assert(::fast_io::is_tuple<storage_t>, "storage_t is not fast_io::tuple");                     // check is tuple

        storage_t parameters{};
    };

    /// @brief get feature parameter type from wasm feature tuple
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval auto get_feature_parameter_type_from_tuple(::fast_io::tuple<Fs...>) noexcept
    {
        return feature_parameter_t<Fs...>{};
    }

    /// @brief Get a reference to a single parameter
    template <::uwvm2::parser::wasm::concepts::wasm_feature Fc, ::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto&& get_curr_feature_parameter(feature_parameter_t<Fs...> & fs_para) noexcept
    {
        static_assert(has_feature_parameter<Fc> && (::std::same_as<Fs, Fc> || ...));  // Check if Fc has feature parameter and if it is one of the Fs
        using fc_para = typename Fc::parameter;
        return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<fc_para>(fs_para);
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature Fc, ::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto&& get_curr_feature_parameter(feature_parameter_t<Fs...> const& fs_para) noexcept
    {
        static_assert(has_feature_parameter<Fc> && (::std::same_as<Fs, Fc> || ...));  // Check if Fc has feature parameter and if it is one of the Fs
        using fc_para = typename Fc::parameter;
        return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<fc_para>(fs_para);
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature Fc, ::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto&& get_curr_feature_parameter(feature_parameter_t<Fs...> && fs_para) noexcept
    {
        static_assert(has_feature_parameter<Fc> && (::std::same_as<Fs, Fc> || ...));  // Check if Fc has feature parameter and if it is one of the Fs
        using fc_para = typename Fc::parameter;
        return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<fc_para>(fs_para);
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature Fc, ::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto&& get_curr_feature_parameter(feature_parameter_t<Fs...> const&& fs_para) noexcept
    {
        static_assert(has_feature_parameter<Fc> && (::std::same_as<Fs, Fc> || ...));  // Check if Fc has feature parameter and if it is one of the Fs
        using fc_para = typename Fc::parameter;
        return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<fc_para>(fs_para);
    }

}  // namespace uwvm2::parser::wasm::concepts
