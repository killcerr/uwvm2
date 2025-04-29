/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
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

#pragma once

#ifdef UWVM_MODULE
import fast_io;
# ifdef UWVM_TIMER
import ulte.utils.debug;
# endif
import ulte.parser.wasm.base;
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.parser.wasm.standard.wasm1.opcode;
import ulte.parser.wasm.binfmt.binfmt_ver1;
import :def;
import :feature_def;
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
# include <fast_io_dsal/vector.h>
# ifdef UWVM_TIMER
#  include <utils/debug/impl.h>
# endif
# include <parser/wasm/base/impl.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/standard/wasm1/opcode/impl.h>
# include <parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
# include "feature_def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::features
{
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct type_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Type"};
        inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_byte>(::ulte::parser::wasm::standard::wasm1::section::section_id::type_sec)};

        ::ulte::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::ulte::parser::wasm::standard::wasm1::features::final_function_type<Fs...>> types{};
    };

    /// @brief Define functions for value_type against wasm1 for checking value_type
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr bool define_check_value_type(
        [[maybe_unused]] ::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::standard::wasm1::type::value_type value_type) noexcept
    {
        switch(value_type)
        {
            case ::ulte::parser::wasm::standard::wasm1::type::value_type::i32: [[fallthrough]];
            case ::ulte::parser::wasm::standard::wasm1::type::value_type::i64: [[fallthrough]];
            case ::ulte::parser::wasm::standard::wasm1::type::value_type::f32: [[fallthrough]];
            case ::ulte::parser::wasm::standard::wasm1::type::value_type::f64: return true;
            default: return false;
        }
    }

    /// @brief      handle type_prefix: "functype"
    /// @details    Separate processing to facilitate reuse in subsequent expansion
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* handle_type_prefix_functype(
        ::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        // ... 60 ?? ?? ...
        //        ^^ section_curr

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;
        using value_type_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::ulte::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> const*;

        ::ulte::parser::wasm::standard::wasm1::features::final_function_type<Fs...> ft{};

        // For safety, add parentheses to indicate the scope
        // parameter
        {
            ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 para_len{};
            auto const [next_para_len, err_para_len]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(para_len))};
            if(err_para_len != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_parameter_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(err_para_len);
            }

            section_curr = reinterpret_cast<::std::byte const*>(next_para_len);

            // ... 60 length ... length ... ...
            //               ^^ section_curr
            // If it's a null return value, there's at least one more parameter, so add 1u
            if(static_cast<::std::size_t>(section_end - section_curr) < para_len + static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(1u))
                [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = para_len;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_parameter_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // set parameters
            ft.parameter.begin = reinterpret_cast<value_type_const_may_alias_ptr>(section_curr);
            section_curr += para_len;
            // ... 60 length ... length ... ...
            //                   ^^ section_curr
            ft.parameter.end = reinterpret_cast<value_type_const_may_alias_ptr>(section_curr);

            // check handler
            static_assert(::ulte::parser::wasm::standard::wasm1::features::has_check_value_type<Fs...>, "define_check_value_type(...) not found");
            // check parameters
            for(auto parameter_curr{ft.parameter.begin}; parameter_curr != ft.parameter.end; ++parameter_curr)
            {
                if(!define_check_value_type(sec_adl, *parameter_curr)) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u8 =
                        static_cast<::std::underlying_type_t<::ulte::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...>>>(*parameter_curr);
                    err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_value_type;
                    ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }
            }
        }

        // result
        {
            ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 result_len{};
            auto const [next_result_len, err_result_len]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                  reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                  ::fast_io::mnp::leb128_get(result_len))};
            if(err_result_len != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_result_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(err_result_len);
            }

            section_curr = reinterpret_cast<::std::byte const*>(next_result_len);
            // ... 60 length ... length ... ...
            //                          ^^ section_curr

            /// @details    In the current version of WebAssembly, the length of the result type vector of a valid function type may be
            ///             at most 1. This restriction may be removed in future versions.
            /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.3.3
            constexpr bool allow_multi_value{::ulte::parser::wasm::standard::wasm1::features::allow_multi_result_vector<Fs...>()};
            if constexpr(!allow_multi_value)
            {
                if(result_len > static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(1u)) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::wasm1_not_allow_multi_value;
                    ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }
            }

            if(static_cast<::std::size_t>(section_end - section_curr) < result_len) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = result_len;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_result_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // set parameters
            ft.result.begin = reinterpret_cast<value_type_const_may_alias_ptr>(section_curr);
            section_curr += result_len;
            // ... 60 length ... length ... ...
            //                              ^^ section_curr
            ft.result.end = reinterpret_cast<value_type_const_may_alias_ptr>(section_curr);

            // check results
            for(auto result_curr{ft.result.begin}; result_curr != ft.result.end; ++result_curr)
            {
                if(!define_check_value_type(sec_adl, *result_curr)) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u8 =
                        static_cast<::std::underlying_type_t<::ulte::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...>>>(*result_curr);
                    err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_value_type;
                    ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }
            }
        }

        // push back
        auto& typesec{::ulte::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        typesec.types.push_back_unchecked(::std::move(ft));

        return section_curr;
    }

    /// @brief Define type_prefix for wasm1 Functions for checking type_prefix
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* define_type_prefix_handler(
        ::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::standard::wasm1::type::function_type_prefix prefix,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        // ... 60 ?? ?? ...
        //        ^^ section_curr

        switch(prefix)
        {
            case ::ulte::parser::wasm::standard::wasm1::type::function_type_prefix::functype:
            {
                return handle_type_prefix_functype(sec_adl, module_storage, section_curr, section_end, err);
            }
            default:
                [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u8 =
                        static_cast<::std::underlying_type_t<::ulte::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...>>>(prefix);
                    err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_type_prefix;
                    ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }
        }
    }

    /// @brief Define the handler function for type_section
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::ulte::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::ulte::utils::debug::timer parsing_timer{u8"parse type section (id: 1)"};
#endif

        // get type_section_storage_t from storages
        auto& typesec{::ulte::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(typesec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = section_begin;
            err.err_selectable.u8 = typesec.section_id;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::ulte::parser::wasm::standard::wasm1::type::wasm_byte const*;

        typesec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        typesec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 type_count{};

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [type_count_next, type_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(type_count))};

        if(type_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_type_count;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(type_count_err);
        }

        typesec.types.reserve(type_count);

        section_curr = reinterpret_cast<::std::byte const*>(type_count_next);  // never out of bounds

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 type_counter{};

        for(; section_curr != section_end;)
        {
            // ... 60 ?? ?? ...
            //     ^^ section_curr
            ::ulte::parser::wasm::standard::wasm1::features::final_type_prefix_t<Fs...> prefix{};
            // no necessary to check, because section_curr != section_end
            ::fast_io::freestanding::my_memcpy(::std::addressof(prefix), section_curr, sizeof(prefix));
            // set section_curr to next
            ++section_curr;
            // ... 60 ?? ?? ...
            //        ^^ section_curr
            // check has func
            static_assert(::ulte::parser::wasm::standard::wasm1::features::has_type_prefix_handler<Fs...>, "define_type_prefix_handler(...) not found");
            // check type count
            if(++type_counter > type_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = type_count;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::type_section_resolved_exceeded_the_actual_number;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
            // handle it
            section_curr = define_type_prefix_handler(sec_adl, prefix, module_storage, section_curr, section_end, err);
        }

        // check type counter match
        if(type_counter != type_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = type_counter;
            err.err_selectable.u32arr[1] = type_count;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::type_section_resolved_exceeded_the_actual_number;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }
}  // namespace ulte::parser::wasm::standard::wasm1::features

UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::ulte::parser::wasm::standard::wasm1::features::type_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::ulte::parser::wasm::standard::wasm1::features::type_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
