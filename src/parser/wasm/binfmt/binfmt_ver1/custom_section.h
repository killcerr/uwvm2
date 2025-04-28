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
 * @date        2025-04-19
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
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.binfmt.base;
import :section;
import :def;
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
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::binfmt::ver1
{
    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_custom_section(::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
                                                            ::std::byte const* section_begin,
                                                            ::std::byte const* section_end,
                                                            ::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        ::parser::wasm::standard::wasm1::section::custom_section cs{};

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::parser::wasm::standard::wasm1::type::wasm_byte const*;
        cs.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        cs.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        // ... | name_length ... name ... custombegin ... | section_end
        //       ^^ section_begin
        ::parser::wasm::standard::wasm1::type::wasm_u32 name_len{};
        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;
        auto const [next_name_len, err_name_len]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_begin),
                                                                          reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                          ::fast_io::mnp::leb128_get(name_len))};

        if(err_name_len != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_begin;
            err.err_code = ::parser::wasm::base::wasm_parse_error_code::invalid_custom_name_length;
            ::parser::wasm::base::throw_wasm_parse_code(err_name_len);
        }

        // ... | name_length ... name ... custombegin ... | section_end
        //                       ^^ next_name_len
        cs.custom_name = ::fast_io::u8string_view{next_name_len, name_len};

        // ... | name_length ... name ... custombegin ... | section_end
        //                                ^^ next_name_len + name_len
        cs.custom_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(next_name_len) + name_len;

        auto& customs{module_storage.custom_sections};
        customs.push_back(::std::move(cs));
    }
}

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
