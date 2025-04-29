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

#if !(__cpp_structured_bindings >= 202411L)
# error "UWVM requires at least C++26 standard compiler."
#endif

#ifdef UWVM_MODULE
import fast_io;
import ulte.parser.wasm.base;
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.parser.wasm.binfmt.base;
import :section;
import :def;
import :custom_section;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <vector>
# include <algorithm>
# include <memory>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/tuple.h>
# include <parser/wasm/base/impl.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/binfmt/base/impl.h>
# include "section.h"
# include "def.h"
# include "custom_section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::binfmt::ver1
{
    static_assert(sizeof(::std::byte) == sizeof(char8_t));

    template <typename Sec, typename... Fs>
    concept has_handle_binfmt_ver1_extensible_section_define =
        requires(::ulte::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec>> ref,
                 ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_begin,
                 ::std::byte const* section_end,
                 ::ulte::parser::wasm::base::error_impl& err) {
            { handle_binfmt_ver1_extensible_section_define(ref, module_storage, section_begin, section_end, err) };
        };

    /// @see WebAssembly Release 1.0 (2019-07-20) § 5.5.2
    template <typename Ty>
    concept has_section_id_define =
        requires { requires std::same_as<std::remove_cvref_t<decltype(Ty::section_id)>, ::ulte::parser::wasm::standard::wasm1::type::wasm_byte>; };

    template <typename Ty>
    concept has_section_name_define = requires { requires std::same_as<std::remove_cvref_t<decltype(Ty::section_name)>, ::fast_io::u8string_view>; };

    template <typename Ty, typename... Fs>
    concept has_section_id_and_handle_binfmt_ver1_extensible_section_define =
        has_handle_binfmt_ver1_extensible_section_define<Ty, Fs...> && has_section_id_define<Ty>;

    template <typename... Ty>
    inline consteval void check_extensible_section_is_series() noexcept
    {
        ::std::vector<::ulte::parser::wasm::standard::wasm1::type::wasm_byte> vec{};
        [&vec]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        { ((vec.push_back(Ty...[I] ::section_id)), ...); }(::std::make_index_sequence<sizeof...(Ty)>{});
        ::std::ranges::sort(vec);
        ::ulte::parser::wasm::standard::wasm1::type::wasm_byte counter{1u};
        for(auto i: vec)
        {
#if __cpp_contracts >= 202502L
            contract_assert(i == counter++);
#else
            if(i != counter++) { ::fast_io::fast_terminate(); }
#endif
        }
    }

    template <typename... Ty>
    inline consteval auto generate_section_name_table_max() noexcept
    {
        ::std::vector<::ulte::parser::wasm::standard::wasm1::type::wasm_byte> vec{};
        [&vec]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        { ((vec.push_back(Ty...[I] ::section_id)), ...); }(::std::make_index_sequence<sizeof...(Ty)>{});
        ::std::ranges::sort(vec);
        auto const max{vec.back()};
        return max;
    }

    template <typename... Ty>
    inline consteval auto generate_section_name_table() noexcept
    {
        constexpr auto max{generate_section_name_table_max<Ty...>};
        ::fast_io::array<::fast_io::u8string_view, max> res{};

        [&res]<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
        { ((res[Ty...[I] ::section_id] = Ty...[I] ::section_name), ...); }(::std::make_index_sequence<sizeof...(Ty)>{});

        return res;
    }

    namespace details
    {
        template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
        inline constexpr void
            handle_all_binfmt_ver1_extensible_section_impl(bool&,
                                                           ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>&,
                                                           ::ulte::parser::wasm::standard::wasm1::type::wasm_byte,
                                                           ::std::byte const*,
                                                           ::std::byte const*,
                                                           ::ulte::parser::wasm::base::error_impl&) noexcept
        {
            // do nothing
        }

        /// @throw ::fast_io::error
        template <typename SecCurr, typename... Secs, ::ulte::parser::wasm::concepts::wasm_feature... Fs>
        inline constexpr void handle_all_binfmt_ver1_extensible_section_impl(
            bool& success,
            ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
            ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id,
            ::std::byte const* section_begin,
            ::std::byte const* section_end,
            ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
        {
            static_assert(has_section_id_and_handle_binfmt_ver1_extensible_section_define<SecCurr, Fs...>);

            /// @details    This can be optimized to jump tables, which is fine under llvm, not gcc or msvc.
            /// @see        test\non-platform-specific\0001.parser\0002.binfmt1\handle_all_binfmt_ver1_extensible_section_test_jmp_table.cc

            if(section_id == SecCurr::section_id)
            {
                handle_binfmt_ver1_extensible_section_define(::ulte::parser::wasm::concepts::feature_reserve_type<::std::remove_cvref_t<SecCurr>>,
                                                             module_storage,
                                                             section_begin,
                                                             section_end,
                                                             err);
                success = true;
            }
            else
            {
                if constexpr(sizeof...(Secs) != 0)
                {
                    handle_all_binfmt_ver1_extensible_section_impl<Secs...>(success, module_storage, section_id, section_begin, section_end, err);
                }
            }
        }

    }  // namespace details

    /// @brief      handle all binfmt ver1 extensible section
    /// @throws     ::fast_io::error
    /// @see        test\non-platform-specific\0001.parser\0002.binfmt1\handle_all_binfmt_ver1_extensible_section.cc
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_all_binfmt_ver1_extensible_section(::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> &
                                                                        module_storage,
                                                                    ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id,
                                                                    ::std::byte const* section_begin,
                                                                    ::std::byte const* section_end,
                                                                    ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        auto [... secs]{module_storage.sections};
        check_extensible_section_is_series<decltype(secs)...>();

        bool success{};

        // llvm can gen jump table here
        if(section_id ==
           static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_byte>(::ulte::parser::wasm::standard::wasm1::section::section_id::custom_sec))
        {
            ::ulte::parser::wasm::binfmt::ver1::handle_binfmt_ver1_custom_section(module_storage, section_begin, section_end, err);
            success = true;
        }
        else
        {
            details::handle_all_binfmt_ver1_extensible_section_impl<decltype(secs)...>(success, module_storage, section_id, section_begin, section_end, err);
        }

        if(!success) [[unlikely]]
        {
            err.err_curr = section_begin;
            err.err_selectable.u8 = section_id;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_section_id;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    [[nodiscard]] inline constexpr wasm_binfmt_ver1_module_extensible_storage_t<Fs...> wasm_binfmt_ver1_handle_func(::fast_io::tuple<Fs...>,
                                                                                                                    ::std::byte const* const module_begin,
                                                                                                                    ::std::byte const* const module_end,
                                                                                                                    ::ulte::parser::wasm::base::error_impl& err)
        UWVM_THROWS
    {
        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        wasm_binfmt_ver1_module_extensible_storage_t<Fs...> ret{};

        ret.module_span.module_begin = module_begin;
        ret.module_span.module_end = module_end;

        ::std::byte const* module_curr{module_begin};

        // 00 61 73 6D 01 00 00 00 01 7D ...
        // ^^ module_curr

        if(static_cast<::std::size_t>(module_end - module_curr) < 8uz || !::ulte::parser::wasm::binfmt::is_wasm_file_unchecked(module_curr)) [[unlikely]]
        {
            err.err_curr = module_curr;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_wasm_file_format;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // uncheck binfmt version, user-selected, or auto-detect

        module_curr += 8uz;

        // 00 61 73 6D 01 00 00 00 01 7D ...
        //                         ^^ module_curr

        if(static_cast<::std::size_t>(module_end - module_curr) < 2uz) [[unlikely]]
        {
            // No need to check module_curr > module_end, always false
            err.err_curr = module_curr;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::no_wasm_section_found;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        for(;;)
        {
            // get section type
            ::ulte::parser::wasm::standard::wasm1::type::wasm_byte sec_id{};
            ::fast_io::freestanding::my_memcpy(::std::addressof(sec_id), module_curr, sizeof(::ulte::parser::wasm::standard::wasm1::type::wasm_byte));

            // get section length
            ++module_curr;
            // 00 61 73 6D 01 00 00 00 01 7D ...
            //                            ^^ module_curr

            ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 sec_len{};
            auto const [sec_len_next, sec_len_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(module_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(module_end),
                                                                            ::fast_io::mnp::leb128_get(sec_len))};
            if(sec_len_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = module_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_section_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(sec_len_err);
            }

            // set curr to next
            module_curr = reinterpret_cast<::std::byte const*>(sec_len_next);

            // check length
            if(static_cast<::std::size_t>(module_end - module_curr) < sec_len) [[unlikely]]
            {
                err.err_curr = module_curr;
                err.err_selectable.u32 = sec_len;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_section_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            auto const sec_end{module_curr + sec_len};

            handle_all_binfmt_ver1_extensible_section(ret, sec_id, module_curr, sec_end, err);

            module_curr = sec_end;

            // check next section
            if(auto const dif{static_cast<::std::size_t>(module_end - module_curr)}; dif == 0uz)
            {
                // there are no sections remaining
                break;
            }
            else if(dif < 2uz) [[unlikely]]
            {
                // No need to check module_curr > module_end, always false
                err.err_curr = module_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::no_enough_space;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        return ret;
    }
}  // namespace ulte::parser::wasm::binfmt::ver1

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
