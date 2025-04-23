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
import utils.io;
import utils.ansies;
# ifdef UWVM_TIMER
import utils.debug;
# endif
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
import :def;
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
# include <utils/ansies/ansi_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/vector.h>
# include <utils/io/impl.h>
# include <utils/ansies/impl.h>
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
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::standard::wasm1::features
{
    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t
    {
        inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 section_id{
            static_cast<::parser::wasm::standard::wasm1::type::wasm_u32>(::parser::wasm::standard::wasm1::section::section_id::import_sec)};

        ::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::parser::wasm::standard::wasm1::features::final_import_type<Fs...>> imports{};
        ::fast_io::array<::fast_io::vector<::parser::wasm::standard::wasm1::features::final_import_type<Fs...> const*>,
                         static_cast<::std::size_t>(::parser::wasm::standard::wasm1::features::final_extern_type_t<Fs...>::extern_type_end) + 1>
            importdesc{};
    };

    /// @brief Define the handler function for type_section
    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline void handle_binfmt_ver1_extensible_section_define(
        [[maybe_unused]] ::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::utils::debug::timer parsing_timer{u8"parse import section (id: 2)"};
#endif
        // get import_section_storage_t from storages
        auto& importsec{::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(importsec.sec_span.sec_begin) [[unlikely]]
        {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
            ::fast_io::io::perr(::utils::log_output,
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"(offset=",
                                ::fast_io::mnp::addrvw(section_begin - module_storage.module_span.module_begin),
                                u8") Duplicate WebAssembly Section: \"",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_CYAN),
                                u8"Import (ID=2)",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"\"",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL),
                                u8"\n\n");
#endif
            ::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // check has typesec
        auto const& typesec{::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};

        if(!typesec.sec_span.sec_begin) [[unlikely]]
        {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
            ::fast_io::io::perr(::utils::log_output,
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"(offset=",
                                ::fast_io::mnp::addrvw(section_begin - module_storage.module_span.module_begin),
                                u8") The \"",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_CYAN),
                                u8"Type",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"\" section must appear before \"",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_CYAN),
                                u8"Import",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"\" section.",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL),
                                u8"\n\n");
#endif
            ::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::parser::wasm::standard::wasm1::type::wasm_byte const*;

        importsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        importsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        ::parser::wasm::standard::wasm1::type::wasm_u32 import_count{};

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [import_count_next, import_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                  reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                  ::fast_io::mnp::leb128_get(import_count))};

        if(import_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
            ::fast_io::io::perr(::utils::log_output,
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_WHITE),
                                u8"(offset=",
                                ::fast_io::mnp::addrvw(section_curr - module_storage.module_span.module_begin),
                                u8") Invalid Import Count.",
                                ::fast_io::mnp::cond(::utils::ansies::put_color, UWVM_AES_U8_RST_ALL),
                                u8"\n\n");
#endif
            ::parser::wasm::base::throw_wasm_parse_code(import_count_err);
        }

        importsec.imports.reserve(import_count);

        /// @todo
        ::fast_io::fast_terminate();
    }
}  // namespace parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <utils/ansies/ansi_pop_macro.h>
# include <utils/macro/pop_macros.h>
#endif
