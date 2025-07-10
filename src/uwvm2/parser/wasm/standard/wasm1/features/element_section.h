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
 * @date        2025-07-03
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

#ifdef UWVM_MODULE
import fast_io;
# ifdef UWVM_TIMER
import uwvm2.utils.debug;
# endif
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.standard.wasm1.section;
import uwvm2.parser.wasm.standard.wasm1.opcode;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import :def;
import :feature_def;
import :types;
#else
// std
# include <cstddef>
# include <cstdint>
# include <cstring>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# ifdef UWVM_TIMER
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
# include "feature_def.h"
# include "types.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct element_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Element"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::element_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_element_type_t<Fs...>> elems{};
    };

    /// @brief Define function for wasm1 wasm1_element_t
    /// @note  ADL for distribution to the correct handler function
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* define_handler_element_type(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<element_section_storage_t<Fs...>> sec_adl,
        decltype(::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>{}.storage)& fet_storage,  // [adl]
        decltype(::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>{}.type) const fet_type,   // [adl]
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) UWVM_THROWS
    {
        // [table_idx ...] expr ... 0x0B func_count ... func ... next_table_idx ...
        // [     safe    ] unsafe (could be the section_end)
        //                 ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_elem_storage_t& wet{fet_storage.table_idx};
        auto const elem_table_idx{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(fet_type)};

        // import section
        auto const& importsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(module_storage.sections)};
        // importdesc[0]: func, importdesc[1]: table
        constexpr ::std::size_t importdesc_count{importsec.importdesc_count};
        static_assert(importdesc_count > 1uz);  // Ensure that subsequent index visits do not cross boundaries

        // get function_storage_t from storages
        auto const& funcsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};
        auto const defined_func_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(funcsec.funcs.size())};
        auto const imported_func_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(importsec.importdesc.index_unchecked(0uz).size())};
        // Addition does not overflow
        auto const all_func_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(defined_func_size + imported_func_size)};

        // get table_section_storage_t from storages
        auto const& tablesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<table_section_storage_t<Fs...>>(module_storage.sections)};
        auto const defined_table_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(tablesec.tables.size())};
        auto const imported_table_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(importsec.importdesc.index_unchecked(1uz).size())};
        // Addition does not overflow
        auto const all_table_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(defined_table_size + imported_table_size)};

        // check table index
        if(elem_table_idx >= all_table_size) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.elem_table_index_exceeds_maxvul.idx = elem_table_idx;
            err.err_selectable.elem_table_index_exceeds_maxvul.maxval = all_table_size;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::elem_table_index_exceeds_maxvul;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        wet.table_idx = elem_table_idx;

        // [table_idx ...] expr ... 0x0B func_count ... func ... next_table_idx ...
        // [     safe    ] unsafe (could be the section_end)
        //                 ^^ section_curr

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        // get expr
        wet.expr.begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_curr);

        // [table_idx ...] expr ... 0x0B func_count ... func ... next_table_idx ...
        // [     safe    ] unsafe (could be the section_end)
        //                 ^^ wet.expr.begin

        for(;; ++section_curr)
        {
            if(section_curr == section_end) [[unlikely]]
            {
                // [table_idx ... ...] (end)
                // [     safe        ] unsafe (could be the section_end)
                //                     ^^ section_curr

                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::elem_init_terminator_not_found;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [... curr] ...
            // [  safe  ] unsafe (could be the section_end)
            //      ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type section_curr_c8;
            ::std::memcpy(::std::addressof(section_curr_c8), section_curr, sizeof(::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type));

#if CHAR_BIT > 8
            section_curr_c8 &= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0xFFu);
#endif

            if(section_curr_c8 == static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0x0Bu)) { break; }
        }

        // [table_idx ... expr ... 0x0B] func_count ... func ... next_table_idx ...
        // [            safe           ] unsafe (could be the section_end)
        //                         ^^ section_curr

        ++section_curr;

        // [table_idx ... expr ... 0x0B] func_count ... func ... next_table_idx ...
        // [            safe           ] unsafe (could be the section_end)
        //                               ^^ section_curr
        //                               ^^ wet.expr.end

        wet.expr.end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_curr);

        // No boundary check is needed here, parse_by_scan comes with its own checks

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 funcidx_count;

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [funcidx_count_next, funcidx_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                    reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                    ::fast_io::mnp::leb128_get(funcidx_count))};

        if(funcidx_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_elem_funcidx_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(funcidx_count_err);
        }

        // [table_idx ... expr ... 0x0B func_count ...] func ... next_table_idx ...
        // [                     safe                 ] unsafe (could be the section_end)
        //                              ^^ section_curr

        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(funcidx_count > size_t_max) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u64 = static_cast<::std::uint_least64_t>(funcidx_count);
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        wet.vec_funcidx.reserve(static_cast<::std::size_t>(funcidx_count));

        section_curr = reinterpret_cast<::std::byte const*>(funcidx_count_next);

        // [table_idx ... expr ... 0x0B func_count ...] func ... next_table_idx ...
        // [                     safe                 ] unsafe (could be the section_end)
        //                                              ^^ section_curr

        for(::std::size_t funcidx_counter{}; funcidx_counter != funcidx_count; ++funcidx_counter)
        {
            // [ ...] func_curr ... func_next ...
            // [safe] unsafe (could be the section_end)
            //        ^^ section_curr

            // No boundary check is needed here, parse_by_scan comes with its own checks

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 funcidx;

            auto const [funcidx_next, funcidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(funcidx))};

            if(funcidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_elem_funcidx;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(funcidx_err);
            }

            // [ ... func_curr ...] func_next ...
            // [       safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            if(funcidx >= all_func_size) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.elem_func_index_exceeds_maxvul.idx = funcidx;
                err.err_selectable.elem_func_index_exceeds_maxvul.maxval = all_func_size;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::elem_func_index_exceeds_maxvul;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            wet.vec_funcidx.push_back_unchecked(funcidx);

            section_curr = reinterpret_cast<::std::byte const*>(funcidx_next);

            // [ ... func_curr ...] func_next ...
            // [       safe       ] unsafe (could be the section_end)
            //                      ^^ section_curr
        }

        return section_curr;
    }

    /// @brief Define the handler function for element_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<element_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::std::byte const* const sec_id_module_ptr) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse element section (id: 9)"};
#endif
        // Note that section_begin may be equal to section_end
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // get element_section_storage_t from storages
        auto& elemsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<element_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(elemsec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8 = elemsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        elemsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        elemsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        // [before_section ... ] | elem_count ...
        // [        safe       ] | unsafe (could be the section_end)
        //                         ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 elem_count;  // No initialization necessary

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [elem_count_next, elem_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(elem_count))};

        if(elem_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_elem_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(elem_count_err);
        }

        // [before_section ... ] | elem_count ...] (end)
        // [        safe                         ] unsafe (could be the section_end)
        //                         ^^ section_curr

        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(elem_count > size_t_max) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u64 = static_cast<::std::uint_least64_t>(elem_count);
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        elemsec.elems.reserve(static_cast<::std::size_t>(elem_count));

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 elem_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(elem_count_next);  // never out of bounds

        // [before_section ... ] | elem_count ...] table_idx
        // [        safe                         ] unsafe (could be the section_end)
        //                                         ^^ section_curr

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [elem_kind] ...
            // [   safe  ] unsafe (could be the section_end)
            //  ^^ section_curr

            // check elem counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++elem_counter > elem_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = elem_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::elem_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // be used to store
            ::uwvm2::parser::wasm::standard::wasm1::features::final_element_type_t<Fs...> fet{};

            // [elem_kind] ...
            // [   safe  ] unsafe (could be the section_end)
            //  ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 elem_kind;  // No initialization necessary

            auto const [elem_kind_next, elem_kind_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                ::fast_io::mnp::leb128_get(elem_kind))};

            if(elem_kind_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                if constexpr(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::features::final_element_type_t<Fs...>,
                                            ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>>)
                {
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_elem_table_idx;
                }
                else
                {
                    // In wasm 1.1 the table idx can only be 0 for expansion using wasm 1.0, so the type of error reported is different
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_elem_kind;
                }
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(elem_kind_err);
            }

            // [elem_kind ...] ...
            // [     safe    ] unsafe (could be the section_end)
            //  ^^ section_curr

            fet.type = static_cast<decltype(fet.type)>(elem_kind);

            section_curr = reinterpret_cast<::std::byte const*>(elem_kind_next);

            // [elem_kind ...] ...
            // [     safe    ] unsafe (could be the section_end)
            //                 ^^ section_curr

            static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_handle_element_type<Fs...>);

            section_curr = define_handler_element_type(sec_adl, fet.storage, fet.type, module_storage, section_curr, section_end, err, fs_para);

            // [elem_kind ... ...] (end)
            // [     safe        ] unsafe (could be the section_end)
            //                     ^^ section_curr

            elemsec.elems.push_back_unchecked(::std::move(fet));
        }

        // [... ] (section_end)
        // [safe] unsafe (could be the section_end)
        //        ^^ section_curr

        // check elem counter match
        if(elem_counter != elem_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = elem_counter;
            err.err_selectable.u32arr[1] = elem_count;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::element_section_resolved_not_match_the_actual_number;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }
}  // namespace uwvm2::parser::wasm::standard::wasm1::features

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::element_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::element_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
