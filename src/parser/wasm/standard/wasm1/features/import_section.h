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
import :type_section;
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
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
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
# include "type_section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::features
{
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Import"};
        inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 section_id{
            static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(::ulte::parser::wasm::standard::wasm1::section::section_id::import_sec)};

        ::ulte::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::ulte::parser::wasm::standard::wasm1::features::final_import_type<Fs...>> imports{};

        inline static constexpr ::std::size_t importdesc_count{
            static_cast<::std::size_t>(decltype(::ulte::parser::wasm::standard::wasm1::features::final_extern_type_t<Fs...>{}.type)::external_type_end) + 1};
        ::fast_io::array<::fast_io::vector<::ulte::parser::wasm::standard::wasm1::features::final_import_type<Fs...> const*>, importdesc_count> importdesc{};
    };

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* extern_imports_func_handler(
        [[maybe_unused]] ::ulte::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*& funcptr_r,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        // ... descindex typeindex ...
        //               ^^ section_curr

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;
        // get type index

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 type_index{};
        auto const [type_index_next, type_index_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(type_index))};
        if(type_index_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(type_index_err);
        }

        auto const& typesec{::ulte::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};

        // check
        if(type_index >= type_section_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32 = type_index;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        funcptr_r = typesec.types.cbegin() + type_index;

        // set curr
        section_curr = reinterpret_cast<::std::byte const*>(type_index_next);

        return section_curr;
    }

    /// @brief Define function for wasm1 external_types
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* define_extern_prefix_imports_handler(
        ::ulte::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::standard::wasm1::features::wasm1_final_extern_type<Fs...> & fit_imports,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        switch(fit_imports.type)
        {
            case ::ulte::parser::wasm::standard::wasm1::type::external_types::func:
            {
                return extern_imports_func_handler(sec_adl, fit_imports.storage.function, module_storage, section_curr, section_end, err);
            }
            case ::ulte::parser::wasm::standard::wasm1::type::external_types::table:
            {
                /// @todo
                break;
            }
            case ::ulte::parser::wasm::standard::wasm1::type::external_types::memory:
            {
                /// @todo
                break;
            }
            case ::ulte::parser::wasm::standard::wasm1::type::external_types::global:
            {
                /// @todo
                break;
            }
            default: ::fast_io::unreachable();  // never match, checked before
        }
        return section_curr;
    }

    /// @brief Define the handler function for type_section
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::ulte::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::ulte::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::ulte::utils::debug::timer parsing_timer{u8"parse import section (id: 2)"};
#endif
        // get import_section_storage_t from storages
        auto& importsec{::ulte::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(importsec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = section_begin;
            err.err_selectable.u32 = importsec.section_id;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // check has typesec
        auto const& typesec{::ulte::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};

        if(!typesec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = section_begin;
            err.err_selectable.u32arr[0] = typesec.section_id;
            err.err_selectable.u32arr[1] = importsec.section_id;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::forward_dependency_missing;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::ulte::parser::wasm::standard::wasm1::type::wasm_byte const*;

        importsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        importsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};
        // ... count namelen ?? ...
        //           ^^ section_curr

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 import_count{};

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [import_count_next, import_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                  reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                  ::fast_io::mnp::leb128_get(import_count))};

        if(import_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_import_count;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(import_count_err);
        }

        importsec.imports.reserve(import_count);

        section_curr = reinterpret_cast<::std::byte const*>(import_count_next);  // never out of bounds

        // ... count ?? ?? ...
        //           ^^ section_curr

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 import_counter{};  // use for check
        ::fast_io::array<::ulte::parser::wasm::standard::wasm1::type::wasm_u32, import_section_storage_t<Fs...>::importdesc_count>
            importdesc_counter{};  // use for reserve

        for(; section_curr != section_end;)
        {
            ::ulte::parser::wasm::standard::wasm1::features::final_import_type<Fs...> fit{};
            if(++import_counter > import_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = import_count;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_section_resolved_exceeded_the_actual_number;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 module_namelen{};
            auto const [module_namelen_next, module_namelen_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                          reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                          ::fast_io::mnp::leb128_get(module_namelen))};

            if(module_namelen_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_import_module_name_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(module_namelen_err);
            }

            if(module_namelen == static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(0u)) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_module_name_length_cannot_be_zero;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            section_curr = reinterpret_cast<::std::byte const*>(module_namelen_next);  // never out of bounds

            // ... count modulenamelen ?? ...
            //                         ^^ section_curr

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            fit.module_name = ::fast_io::u8string_view{reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr), module_namelen};

            section_curr += module_namelen;
            // ... count modulenamelen name ... externnamelen externname ... index
            //                                  ^^ section_curr

            // check curr: externnamelen min = 0, minimum remaining 2 "... 00 index ..."
            if(static_cast<::std::size_t>(section_end - section_curr) < 2uz || section_curr > section_end) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_module_name_too_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 extern_namelen{};
            auto const [extern_namelen_next, extern_namelen_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                          reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                          ::fast_io::mnp::leb128_get(extern_namelen))};

            if(extern_namelen_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::invalid_import_extern_name_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(extern_namelen_err);
            }

            if(extern_namelen == static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(0u)) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_extern_name_length_cannot_be_zero;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            section_curr = reinterpret_cast<::std::byte const*>(extern_namelen_next);  // never out of bounds

            fit.extern_name = ::fast_io::u8string_view{reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr), extern_namelen};

            section_curr += extern_namelen;
            // ... count modulenamelen name ... externnamelen externname ... index
            //                                                               ^^ section_curr

            // same as (static_cast<::std::size_t>(section_end - section_curr) < 1uz || section_curr > section_end)
            if(section_curr >= section_end) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_extern_name_too_length;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            ::fast_io::freestanding::my_memcpy(::std::addressof(fit.imports.type), section_curr, sizeof(fit.imports.type));

            // desc counter
            constexpr ::std::size_t importdesc_count{importsec.importdesc_count};
            // importdesc_count never > 256 (max=255+1), convert to unsigned
            if(static_cast<unsigned>(fit.imports.type) >= static_cast<unsigned>(importdesc_count)) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u8 = static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_byte>(fit.imports.type);
                err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::illegal_importdesc_prefix;
                ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // use for reserve
            ++importdesc_counter.index_unchecked(static_cast<::std::size_t>(static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_byte>(fit.imports.type)));

            ++section_curr;
            // ... count modulenamelen name ... externnamelen externname ... index ...
            //                                                                     ^^ section_curr

            static_assert(::ulte::parser::wasm::standard::wasm1::features::has_extern_prefix_imports_handler<Fs...>,
                          "define_extern_prefix_imports_handler(...) not found");
            // handle it, fit.imports.type is always valid
            section_curr = define_extern_prefix_imports_handler(sec_adl, fit.imports, module_storage, section_curr, section_end, err);

            importsec.imports.push_back_unchecked(::std::move(fit));
        }

        // check import counter match
        if(import_counter != import_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = import_counter;
            err.err_selectable.u32arr[1] = import_count;
            err.err_code = ::ulte::parser::wasm::base::wasm_parse_error_code::import_section_resolved_exceeded_the_actual_number;
            ::ulte::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        /// @todo reserve and pushback
    }
}  // namespace ulte::parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
