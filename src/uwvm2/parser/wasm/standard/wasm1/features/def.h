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
 * @date        2025-04-16
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

#if !(__cpp_pack_indexing >= 202311L)
# error "UWVM requires at least C++26 standard compiler. See https://en.cppreference.com/w/cpp/feature_test#cpp_pack_indexing"
#endif

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <climits>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <compare>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/hash/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <uwvm2/parser/wasm/text_format/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    ///
    /// @brief Define structures or concepts related to the version of binfmt
    ///

    ///////////////////////////
    //     Binary Format     //
    ///////////////////////////

    /// @brief      has text format
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using text_format = type_replacer<root_of_replacement, text_format_wapper<text_format::xxx>>;
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_text_format = requires {
        typename FeatureType::text_format;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::text_format>;
    };

    template <typename FeatureType>
    inline consteval auto get_text_format() noexcept
    {
        if constexpr(has_text_format<FeatureType>) { return typename FeatureType::text_format{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_text_format_wapper = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_text_format<Fs>())...>;

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval auto get_final_text_format_wapper_from_tuple(::uwvm2::utils::container::tuple<Fs...>) noexcept
    {
        return final_text_format_wapper<Fs...>{};
    }

    ////////////////////////////
    //      Type Section      //
    ////////////////////////////

    /// @brief      has value type
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using value_type = type_replacer<root_of_replacement, value_type>;
    ///             };
    ///             ```
    /// @see        test\0001.parser\0002.binfmt1\section\type_section.cc
    template <typename FeatureType>
    concept has_value_type = requires {
        typename FeatureType::value_type;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::value_type>;
    };

    template <typename FeatureType>
    inline consteval auto get_value_type() noexcept
    {
        if constexpr(has_value_type<FeatureType>) { return typename FeatureType::value_type{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_value_type_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_value_type<Fs>())...>;

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct vec_value_type
    {
        static_assert(::std::is_enum_v<final_value_type_t<Fs...>>, "final_value_type_t<Fs...> is not enumeration");
        static_assert(::std::same_as<::std::underlying_type_t<final_value_type_t<Fs...>>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>,
                      "final_value_type_t<Fs...> enumeration is not base on wasm_byte");

        final_value_type_t<Fs...> const* begin{};
        final_value_type_t<Fs...> const* end{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_result_type = vec_value_type<Fs...>;

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_function_type
    {
        final_result_type<Fs...> parameter{};
        final_result_type<Fs...> result{};
    };

    /// @brief      has type prefie
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using type_prefix = type_replacer<root_of_replacement, type_prefix>;
    ///             };
    ///             ```
    /// @see        test\0001.parser\0002.binfmt1\section\type_section.cc
    template <typename FeatureType>
    concept has_type_prefix = requires {
        typename FeatureType::type_prefix;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::type_prefix>;
    };

    template <typename FeatureType>
    inline consteval auto get_type_prefix() noexcept
    {
        if constexpr(has_type_prefix<FeatureType>) { return typename FeatureType::type_prefix{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_type_prefix_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_type_prefix<Fs>())...>;

    /// @brief      allow multi value
    /// @details    In the current version of WebAssembly, the length of the result type vector of a valid function type may be
    ///             at most 1. This restriction may be removed in future versions.
    ///
    ///             Define this to eliminate checking the length of the result.
    ///
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 inline static constexpr bool allow_multi_result_vector{true};
    ///             };
    ///             ```
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.3.3
    /// @see        test\0001.parser\0002.binfmt1\section\type_section.cc
    template <typename FsCurr>
    concept has_allow_multi_result_vector = requires { requires ::std::same_as<::std::remove_cvref_t<decltype(FsCurr::allow_multi_result_vector)>, bool>; };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool allow_multi_result_vector() noexcept
    {
        return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            return ((
                        []<typename FsCurr>() constexpr noexcept -> bool
                                                                    {
                                                                        // check irreplaceable
                                                                        if constexpr(has_allow_multi_result_vector<FsCurr>)
                                                                        {
                                                                            constexpr bool tallow{FsCurr::allow_multi_result_vector};
                                                                            return tallow;
                                                                        }
                                                                        else
                                                                        {
                                                                            return false;
                                                                        }
                                                                    }.template operator()<Fs...[I]>()) ||
                    ...);
        }(::std::make_index_sequence<sizeof...(Fs)>{});
    }

    /// @brief      controllable allow multi value
    /// @details    Use a runtime boolean variable to control whether to start the check.
    ///
    ///             Prerequisites, satisfied:
    ///             - allow_multi_result_vector<Fs...>
    ///
    /// @see        allow_multi_result_vector
    /// @see        test/0001.parser/0002.binfmt1/section/type_section.cc
    template <typename FsCurr>
    concept has_curr_feature_parameter_controllable_allow_multi_result_vector = requires(FsCurr const& curr_feature_para) {
        requires ::std::same_as<::std::remove_cvref_t<decltype(curr_feature_para.controllable_allow_multi_result_vector)>, bool>;
    };

    template <typename... Para>
    inline consteval bool has_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters(
        ::uwvm2::utils::container::tuple<Para...> const&) noexcept
    {
        // e.g.
        // tuple<struct{bool controllable_allow_multi_result_vector{};}, ...> -> true
        // tuple<struct{bool controllable_allow_multi_result_vector{};}, struct{bool controllable_allow_multi_result_vector{};}, ...> -> contract_assert(false)

        bool has{};

        // Using double lambdas in consteval does not affect performance.
        [&]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            ((
                 [&]<typename CurrParaType>() constexpr noexcept
                 {
                     if constexpr(has_curr_feature_parameter_controllable_allow_multi_result_vector<CurrParaType>)
                     {
#if __cpp_contracts >= 202502L
                         contract_assert(!has);
#else
                         if(has) { ::fast_io::fast_terminate(); }
#endif
                         has = true;
                     }
                 }.template operator()<Para...[I]>()),
             ...);
        }(::std::make_index_sequence<sizeof...(Para)>{});

        return has;
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool has_feature_parameter_controllable_allow_multi_result_vector_from_paras(
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& paras) noexcept
    {
        return has_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters(paras.parameters);
    }

    template <typename... Fs>
    concept has_feature_parameter_controllable_allow_multi_result_vector_from_paras_c =
        has_feature_parameter_controllable_allow_multi_result_vector_from_paras<Fs...>(::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...>{});

    /// @brief      get controllable allow multi value
    /// @details    Get the first controllable allow multi value from the parameters.
    /// @see        has_feature_parameter_controllable_allow_multi_result_vector_from_paras_c
    /// @see        test/0001.parser/0002.binfmt1/section/type_section.cc
    template <::std::size_t N, typename... Paras>
    inline constexpr bool get_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters_impl(
        ::uwvm2::utils::container::tuple<Paras...> const& paras) noexcept
    {
        if constexpr(N >= sizeof...(Paras)) { return false; }
        else
        {
            auto const& curr_para{get<N>(paras)};
            using curr_para_t = ::std::remove_cvref_t<decltype(curr_para)>;
            if constexpr(has_curr_feature_parameter_controllable_allow_multi_result_vector<curr_para_t>)
            {
                return curr_para.controllable_allow_multi_result_vector;
            }
            else
            {
                return get_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters_impl<N + 1uz>(paras);
            }
        }
    }

    template <typename... Para>
    inline constexpr bool get_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters(
        ::uwvm2::utils::container::tuple<Para...> const& paras) noexcept
    {
        return get_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters_impl<0uz>(paras);
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        requires (has_feature_parameter_controllable_allow_multi_result_vector_from_paras_c<Fs...>)
    inline constexpr bool get_feature_parameter_controllable_allow_multi_result_vector_from_paras(
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& paras) noexcept
    {
        return get_feature_parameter_controllable_allow_multi_result_vector_from_paras_parameters(paras.parameters);
    }

    /////////////////////////////
    //      Import Section     //
    /////////////////////////////

    /// @brief      has extern type
    /// @details
    ///             ```cpp
    ///             enum class basic_external_type : wasm_byte
    ///             {
    ///                 e1 = 0x00u,
    ///                 e2 = 0x01u,
    ///                 external_type_end = e2 // Used to generate the length of the array
    ///             };
    ///
    ///             template <typename ... Fs> // Fs is used as an extension to an existing type, but does not extend the type
    ///             struct basic_extern_type
    ///             {
    ///                 union storage_t
    ///                 {
    ///                     basic_function_type<Fs ...> const* function;
    ///                     basic_table_type<Fs ...> table;
    ///                     basic_memory_type<Fs ...> memory;
    ///                     basic_global_type<Fs ...> global;
    ///                 } storage;
    ///
    ///                 basic_external_type type{};
    ///             };
    ///
    ///             struct F
    ///             {
    ///                 template <typename ... Fs>
    ///                 using extern_type = type_replacer<root_of_replacement, basic_extern_type<Fs...>>;
    ///             };
    ///             ```
    template <typename FeatureType, typename... Fs>
    concept has_extern_type = requires {
        typename FeatureType::template extern_type<Fs...>;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::template extern_type<Fs...>>;
    };

    template <typename FeatureType, typename... Fs>
    inline consteval auto get_extern_type() noexcept
    {
        if constexpr(has_extern_type<FeatureType>) { return typename FeatureType::template extern_type<Fs...>{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_extern_type_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_extern_type<Fs, Fs...>())...>;

    template <typename... Fs>
    concept is_valid_final_extern_type_t = requires(final_extern_type_t<Fs...> ext) {
        requires ::std::is_enum_v<decltype(ext.type)>;
        requires ::std::same_as<::std::underlying_type_t<decltype(ext.type)>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>;
        decltype(ext.type)::external_type_end;
        requires ::std::is_union_v<decltype(ext.storage)>;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_import_type
    {
        static_assert(is_valid_final_extern_type_t<Fs...>);

        ::uwvm2::utils::container::u8string_view module_name{};
        ::uwvm2::utils::container::u8string_view extern_name{};

        final_extern_type_t<Fs...> imports{};
    };

    /// @brief      No duplicate import items allowed
    /// @details    Currently, the WebAssembly core specification does not prohibit the existence of identical import items (same module name + function name +
    ///             type) in a module, but some host environments require that import object keys be unique when instantiating, causing duplicate imports to
    ///             fail during actual runtime. Here is a concept that can prohibit duplicate import items.
    ///
    ///             Here, modules and names are bound together for duplicate checks. It is permissible to import different modules with the same name or
    ///             different names within the same module.
    ///
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 inline static constexpr bool check_duplicate_imports{true};
    ///             };
    ///             ```
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.5.11
    /// @note       Unlike export names, import names are not necessarily unique. It is possible to import the same module/name pair multiple times; such
    ///             imports may even have different type descriptions, including different kinds of entities. A module with such imports can still be
    ///             instantiated depending on the specifics of how an embedder allows resolving and supplying imports. However, embedders are not required to
    ///             support such overloading, and a WebAssembly module itself cannot implement an overloaded name
    template <typename FsCurr>
    concept has_check_duplicate_imports = requires { requires ::std::same_as<::std::remove_cvref_t<decltype(FsCurr::check_duplicate_imports)>, bool>; };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool check_duplicate_imports() noexcept
    {
        return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            return ((
                        []<typename FsCurr>() constexpr noexcept -> bool
                                                                    {
                                                                        // check irreplaceable
                                                                        if constexpr(has_check_duplicate_imports<FsCurr>)
                                                                        {
                                                                            constexpr bool tallow{FsCurr::check_duplicate_imports};
                                                                            return tallow;
                                                                        }
                                                                        else
                                                                        {
                                                                            return false;
                                                                        }
                                                                    }.template operator()<Fs...[I]>()) ||
                    ...);
        }(::std::make_index_sequence<sizeof...(Fs)>{});
    }

    /// @brief      controllable check duplicate imports
    /// @details    Use a runtime boolean variable to control whether to start the check.
    ///
    ///             Prerequisites, satisfied:
    ///             - check_duplicate_imports<Fs...>
    ///
    /// @see        check_duplicate_imports
    /// @see        test/0001.parser/0002.binfmt1/section/import_section.cc
    template <typename FsCurr>
    concept has_curr_feature_parameter_controllable_check_duplicate_imports = requires(FsCurr const& curr_feature_para) {
        requires ::std::same_as<::std::remove_cvref_t<decltype(curr_feature_para.controllable_check_duplicate_imports)>, bool>;
    };

    template <typename... Para>
    inline consteval bool has_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters(
        ::uwvm2::utils::container::tuple<Para...> const&) noexcept
    {
        // e.g.
        // tuple<struct{bool controllable_check_duplicate_imports{};}, ...> -> true
        // tuple<struct{bool controllable_check_duplicate_imports{};}, struct{bool controllable_check_duplicate_imports{};}, ...> -> contract_assert(false)

        bool has{};

        // Using double lambdas in consteval does not affect performance.
        [&]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            ((
                 [&]<typename CurrParaType>() constexpr noexcept
                 {
                     if constexpr(has_curr_feature_parameter_controllable_check_duplicate_imports<CurrParaType>)
                     {
#if __cpp_contracts >= 202502L
                         contract_assert(!has);
#else
                         if(has) { ::fast_io::fast_terminate(); }
#endif
                         has = true;
                     }
                 }.template operator()<Para...[I]>()),
             ...);
        }(::std::make_index_sequence<sizeof...(Para)>{});

        return has;
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool has_feature_parameter_controllable_check_duplicate_imports_from_paras(
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& paras) noexcept
    {
        return has_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters(paras.parameters);
    }

    template <typename... Fs>
    concept has_feature_parameter_controllable_check_duplicate_imports_from_paras_c =
        has_feature_parameter_controllable_check_duplicate_imports_from_paras<Fs...>(::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...>{});

    /// @brief      get controllable check duplicate imports
    /// @details    Get the first controllable check duplicate imports from the parameters.
    /// @see        has_feature_parameter_controllable_check_duplicate_imports_from_paras_c
    /// @see        test/0001.parser/0002.binfmt1/section/import_section.cc
    template <::std::size_t N, typename... Paras>
    inline constexpr bool get_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters_impl(
        ::uwvm2::utils::container::tuple<Paras...> const& paras) noexcept
    {
        if constexpr(N >= sizeof...(Paras)) { return false; }
        else
        {
            auto const& curr_para{get<N>(paras)};
            using curr_para_t = ::std::remove_cvref_t<decltype(curr_para)>;
            if constexpr(has_curr_feature_parameter_controllable_check_duplicate_imports<curr_para_t>)
            {
                return curr_para.controllable_check_duplicate_imports;
            }
            else
            {
                return get_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters_impl<N + 1uz>(paras);
            }
        }
    }

    template <typename... Para>
    inline constexpr bool get_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters(
        ::uwvm2::utils::container::tuple<Para...> const& paras) noexcept
    {
        return get_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters_impl<0uz>(paras);
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
        requires (has_feature_parameter_controllable_check_duplicate_imports_from_paras_c<Fs...>)
    inline constexpr bool get_feature_parameter_controllable_check_duplicate_imports_from_paras(
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& paras) noexcept
    {
        return get_feature_parameter_controllable_check_duplicate_imports_from_paras_parameters(paras.parameters);
    }

    /// @brief      has table_type
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using table_type = type_replacer<root_of_replacement, table_type>;
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_table_type = requires {
        typename FeatureType::table_type;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::table_type>;
    };

    template <typename FeatureType>
    inline consteval auto get_table_type() noexcept
    {
        if constexpr(has_table_type<FeatureType>) { return typename FeatureType::table_type{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_table_type = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_table_type<Fs>())...>;

    /// @brief      has memory_type
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using memory_type = type_replacer<root_of_replacement, memory_type>;
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_memory_type = requires {
        typename FeatureType::memory_type;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::memory_type>;
    };

    template <typename FeatureType>
    inline consteval auto get_memory_type() noexcept
    {
        if constexpr(has_memory_type<FeatureType>) { return typename FeatureType::memory_type{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_memory_type = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_memory_type<Fs>())...>;

    /// @brief      has global_type
    /// @details
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 using global_type = type_replacer<root_of_replacement, global_type>;
    ///             };
    ///             ```
    template <typename FeatureType>
    concept has_global_type = requires {
        typename FeatureType::global_type;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::global_type>;
    };

    template <typename FeatureType>
    inline consteval auto get_global_type() noexcept
    {
        if constexpr(has_global_type<FeatureType>) { return typename FeatureType::global_type{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_global_type = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_global_type<Fs>())...>;

    /// @brief name checker
    struct name_checker
    {
        ::uwvm2::utils::container::u8string_view module_name{};
        ::uwvm2::utils::container::u8string_view extern_name{};
    };

    inline constexpr bool operator== (name_checker const& n1, name_checker const& n2) noexcept
    {
        return n1.module_name == n2.module_name && n1.extern_name == n2.extern_name;
    }

    inline constexpr ::std::strong_ordering operator<=> (name_checker const& n1, name_checker const& n2) noexcept
    {
        ::std::strong_ordering const module_name_check{n1.module_name <=> n2.module_name};

        if(module_name_check != ::std::strong_ordering::equal) { return module_name_check; }

        return n1.extern_name <=> n2.extern_name;
    }

    /////////////////////////////
    //     Function Section    //
    /////////////////////////////

    /// @brief      final_local_function_type
    /// @details    Storing pointers instead of u32 doubles the memory on 64-bit platforms, and then the index * size is pre-calculated by simd. on 32-bit
    ///             platforms, the memory footprint is the same. On all platforms, calculating the index * size at the same time as the parse reduces the time
    ///             required for subsequent index * size calculations.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_local_function_type
    {
        final_function_type<Fs...> const* func_type{};
    };

    /////////////////////////////
    //      Table Section      //
    /////////////////////////////

    /// @brief      allow multi table
    /// @details    In the current version of WebAssembly, at most one table may be defined or imported in a single module,
    ///             and all constructs implicitly reference this table 0. This restriction may be lifted in future versions.
    ///
    ///             Define this to eliminate checking the length of the result.
    ///
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 inline static constexpr bool allow_multi_table{true};
    ///             };
    ///             ```
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.5.4
    /// @see        test\0001.parser\0002.binfmt1\section\table_section.cc
    template <typename FsCurr>
    concept has_allow_multi_table = requires { requires ::std::same_as<::std::remove_cvref_t<decltype(FsCurr::allow_multi_table)>, bool>; };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool allow_multi_table() noexcept
    {
        return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            return ((
                        []<typename FsCurr>() constexpr noexcept -> bool
                                                                    {
                                                                        // check irreplaceable
                                                                        if constexpr(has_allow_multi_table<FsCurr>)
                                                                        {
                                                                            constexpr bool tallow{FsCurr::allow_multi_table};
                                                                            return tallow;
                                                                        }
                                                                        else
                                                                        {
                                                                            return false;
                                                                        }
                                                                    }.template operator()<Fs...[I]>()) ||
                    ...);
        }(::std::make_index_sequence<sizeof...(Fs)>{});
    }

    // Since multi-table changes the parsing behavior, wasm1.1 uses the features of wasm1.0 for extension, so a freely controllable version is not provided here.

    /////////////////////////////
    //      Memory Section     //
    /////////////////////////////

    /// @brief      allow multi memory
    /// @details    In the current version of WebAssembly, at most one memory may be defined or imported in a single module,
    ///             and all constructs implicitly reference this memory 0. This restriction may be lifted in future versions
    ///
    ///             Define this to eliminate checking the length of the result.
    ///
    ///             ```cpp
    ///             struct F
    ///             {
    ///                 inline static constexpr bool allow_multi_memory{true};
    ///             };
    ///             ```
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.5.5
    /// @see        test\0001.parser\0002.binfmt1\section\memory_section.cc
    template <typename FsCurr>
    concept has_allow_multi_memory = requires { requires ::std::same_as<::std::remove_cvref_t<decltype(FsCurr::allow_multi_memory)>, bool>; };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval bool allow_multi_memory() noexcept
    {
        return []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        {
            return ((
                        []<typename FsCurr>() constexpr noexcept -> bool
                                                                    {
                                                                        // check irreplaceable
                                                                        if constexpr(has_allow_multi_memory<FsCurr>)
                                                                        {
                                                                            constexpr bool tallow{FsCurr::allow_multi_memory};
                                                                            return tallow;
                                                                        }
                                                                        else
                                                                        {
                                                                            return false;
                                                                        }
                                                                    }.template operator()<Fs...[I]>()) ||
                    ...);
        }(::std::make_index_sequence<sizeof...(Fs)>{});
    }

    // Since multi-memory changes the parsing behavior, wasm1.1 uses the features of wasm1.0 for extension, so a freely controllable version is not provided here.

    /////////////////////////////
    //      Global Section     //
    /////////////////////////////

    struct global_expr
    {
        // Expressions are encoded by their instruction sequence terminated with an explicit 0x0B opcode for end.
        inline static constexpr auto end_opcode{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0x0Bu)};

        ::std::byte const* begin;
        ::std::byte const* end;  // The pointer to end is after 0x0b.
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_local_global_type
    {
        final_global_type<Fs...> global;  // Types used to store global
        global_expr expr;                 // Expressions used to initialize global
    };

    /////////////////////////////
    //      Export Section     //
    /////////////////////////////

    /// @brief      has export type
    /// @details
    ///             ```cpp
    ///             enum class basic_external_type : wasm_byte
    ///             {
    ///                 e1 = 0x00u,
    ///                 e2 = 0x01u,
    ///                 external_type_end = e2 // Used to generate the length of the array
    ///             };
    ///
    ///             template <typename ... Fs> // Fs is used as an extension to an existing type, but does not extend the type
    ///             struct basic_export_type
    ///             {
    ///                 union export_storage_t storage{};
    ///
    ///                 basic_external_type type{};
    ///             };
    ///
    ///             struct F
    ///             {
    ///                 template <typename ... Fs>
    ///                 using export_type = type_replacer<root_of_replacement, basic_export_type<Fs...>>;
    ///             };
    ///             ```
    template <typename FeatureType, typename... Fs>
    concept has_export_type = requires {
        typename FeatureType::template export_type<Fs...>;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::template export_type<Fs...>>;
    };

    template <typename FeatureType, typename... Fs>
    inline consteval auto get_export_type() noexcept
    {
        if constexpr(has_export_type<FeatureType>) { return typename FeatureType::template export_type<Fs...>{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_export_type_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_export_type<Fs, Fs...>())...>;

    template <typename... Fs>
    concept is_valid_final_export_type_t = requires(final_export_type_t<Fs...> ext) {
        requires ::std::is_enum_v<decltype(ext.type)>;
        requires ::std::same_as<::std::underlying_type_t<decltype(ext.type)>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>;
        decltype(ext.type)::external_type_end;
        requires ::std::is_union_v<decltype(ext.storage)>;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_wasm_export_type
    {
        static_assert(is_valid_final_export_type_t<Fs...>);

        ::uwvm2::utils::container::u8string_view export_name{};

        final_export_type_t<Fs...> exports{};
    };

    //////////////////////////////
    //      Element Section     //
    //////////////////////////////

    /// @brief      has element type
    /// @details
    ///             ```cpp
    ///             enum class basic_element_type : wasm_u32
    ///             {
    ///                 e1 = 0x00u,
    ///                 e2 = 0x01u,
    ///             };
    ///
    ///             template <typename ... Fs> // Fs is used as an extension to an existing type, but does not extend the type
    ///             struct basic_element_t
    ///             {
    ///                 union storage_t
    ///                 {
    ///                     // ...
    ///                 } storage;
    ///
    ///                 basic_element_type type{};
    ///             };
    ///
    ///             struct F
    ///             {
    ///                 template <typename ... Fs>
    ///                 using element_type = type_replacer<root_of_replacement, basic_element_t<Fs...>>;
    ///             };
    ///             ```
    template <typename FeatureType, typename... Fs>
    concept has_element_type = requires {
        typename FeatureType::template element_type<Fs...>;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::template element_type<Fs...>>;
    };

    template <typename FeatureType, typename... Fs>
    inline consteval auto get_element_type() noexcept
    {
        if constexpr(has_element_type<FeatureType>) { return typename FeatureType::template element_type<Fs...>{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_element_type_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_element_type<Fs, Fs...>())...>;

    template <typename... Fs>
    concept is_valid_final_element_type_t = requires(final_element_type_t<Fs...> ext) {
        requires ::std::is_enum_v<decltype(ext.type)>;
        requires ::std::same_as<::std::underlying_type_t<decltype(ext.type)>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>;
        requires ::std::is_union_v<decltype(ext.storage)>;
    };

    ////////////////////////////
    //      Data Section      //
    ////////////////////////////

    /// @brief      has data type
    /// @details
    ///             ```cpp
    ///             enum class basic_data_type : wasm_u32
    ///             {
    ///                 e1 = 0x00u,
    ///                 e2 = 0x01u,
    ///             };
    ///
    ///             template <typename ... Fs> // Fs is used as an extension to an existing type, but does not extend the type
    ///             struct basic_data_t
    ///             {
    ///                 union storage_t
    ///                 {
    ///                     // ...
    ///                 } storage;
    ///
    ///                 basic_data_type type{};
    ///             };
    ///
    ///             struct F
    ///             {
    ///                 template <typename ... Fs>
    ///                 using data_type = type_replacer<root_of_replacement, basic_data_t<Fs...>>;
    ///             };
    ///             ```
    template <typename FeatureType, typename... Fs>
    concept has_data_type = requires {
        typename FeatureType::template data_type<Fs...>;
        requires ::uwvm2::parser::wasm::concepts::operation::details::check_is_type_replacer<::uwvm2::parser::wasm::concepts::operation::type_replacer,
                                                                                             typename FeatureType::template data_type<Fs...>>;
    };

    template <typename FeatureType, typename... Fs>
    inline consteval auto get_data_type() noexcept
    {
        if constexpr(has_data_type<FeatureType>) { return typename FeatureType::template data_type<Fs...>{}; }
        else
        {
            return ::uwvm2::parser::wasm::concepts::operation::irreplaceable_t{};
        }
    }

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    using final_data_type_t = ::uwvm2::parser::wasm::concepts::operation::replacement_structure_t<decltype(get_data_type<Fs, Fs...>())...>;

    template <typename... Fs>
    concept is_valid_final_data_type_t = requires(final_data_type_t<Fs...> ext) {
        requires ::std::is_enum_v<decltype(ext.type)>;
        requires ::std::same_as<::std::underlying_type_t<decltype(ext.type)>, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>;
        requires ::std::is_union_v<decltype(ext.storage)>;
    };
}

UWVM_MODULE_EXPORT namespace std
{
    template <>
    struct hash<::uwvm2::parser::wasm::standard::wasm1::features::name_checker>
    {
        inline constexpr ::std::size_t operator() (::uwvm2::parser::wasm::standard::wasm1::features::name_checker const& checker) const noexcept
        {
#if CHAR_BIT == 8
            ::std::size_t module_name_sz;
            ::std::size_t extern_name_sz;
            if constexpr(requires { checker.module_name.size_bytes(); }) { module_name_sz = checker.module_name.size_bytes(); }
            else
            {
                module_name_sz = checker.module_name.size() * sizeof(char8_t);
            }

            if constexpr(requires { checker.extern_name.size_bytes(); }) { extern_name_sz = checker.extern_name.size_bytes(); }
            else
            {
                extern_name_sz = checker.extern_name.size() * sizeof(char8_t);
            }

            auto const seed1{::uwvm2::utils::hash::xxh3_64bits(reinterpret_cast<::std::byte const*>(checker.module_name.data()), module_name_sz)};

            return static_cast<::std::size_t>(
                ::uwvm2::utils::hash::xxh3_64bits(reinterpret_cast<::std::byte const*>(checker.extern_name.data()), extern_name_sz, seed1));
#else
            // use std hash

            ::std::size_t h1{::std::hash<::uwvm2::utils::container::u8string_view>{}(checker.module_name)};
            ::std::size_t h2{::std::hash<::uwvm2::utils::container::u8string_view>{}(checker.extern_name)};
            return static_cast<::std::size_t>(h1 ^ (h2 + 0x9e3779b9u + (h1 << 6u) + (h1 >> 2u) + (h2 << 16u)));
#endif
        }
    };
}

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_import_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_local_function_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::global_expr>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_local_global_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_wasm_export_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
