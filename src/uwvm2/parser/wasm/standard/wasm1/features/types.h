/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-03
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
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
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
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    /// @brief      Limits
    /// @details    Limits classify the size range of resizeable storage associated with memory types and table types.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.3.4
    inline constexpr ::std::byte const* scan_limit_type(::uwvm2::parser::wasm::standard::wasm1::type::limits_type & limit_r,
                                                        ::std::byte const* const begin,
                                                        ::std::byte const* const end,
                                                        ::uwvm2::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        // uncheck begin > end
        auto curr{begin};

        // flag min ... (max ...)
        // unsafe (could be the section_end)
        // ^^ curr

        if(curr == end) [[unlikely]]
        {
            err.err_curr = curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_cannot_find_flag;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // [flag] min ... (max ...)
        // [safe] unsafe (could be the section_end)
        // ^^ curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte flags{};
        ::fast_io::freestanding::my_memcpy(::std::addressof(flags), curr, sizeof(flags));

        if(flags != 0u && flags != 1u) [[unlikely]]
        {
            err.err_curr = curr;
            err.err_selectable.u8 = flags;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_illegal_flag;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        ++curr;

        // [flag] min ... max (end)
        // [safe] unsafe (could be the section_end)
        //        ^^ curr

        {
            // [flag] min ... max (end)
            // [safe] unsafe (could be the section_end)
            //        ^^ curr

            // scan min
            limit_r.present_max = false;

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 n_min{};
            auto const [next_n_min, err_n_min]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(end),
                                                                        ::fast_io::mnp::leb128_get(n_min))};

            if(err_n_min != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_invalid_min;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(err_n_min);
            }

            // [flag min ...] max (end)
            // [    safe    ] unsafe (could be the section_end)
            //       ^^ curr

            limit_r.min = n_min;

            curr = reinterpret_cast<::std::byte const*>(next_n_min);
            // [flag min ...] max (end)
            // [    safe    ] unsafe (could be the section_end)
            //                ^^ curr
        }

        if(flags == 1u)
        {
            // [flag min ...] max ... (end)
            // [    safe    ] unsafe (could be the section_end)
            //                ^^ curr

            // scan max
            limit_r.present_max = true;

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 n_max{};
            auto const [next_n_max, err_n_max]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(end),
                                                                        ::fast_io::mnp::leb128_get(n_max))};

            if(err_n_max != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::limit_type_invalid_max;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(err_n_max);
            }

            // [flag min ... max ...] (end)
            // [        safe        ] unsafe (could be the section_end)
            //               ^^ curr

            limit_r.max = n_max;

            curr = reinterpret_cast<::std::byte const*>(next_n_max);

            // [flag min ... max ...] (end)
            // [        safe        ] unsafe (could be the section_end)
            //                        ^^ curr
        }

        return curr;
    }

    /// @brief      Table Types
    /// @details    Memory types classify linear memories and their size range.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.3.6
    inline constexpr ::std::byte const* scan_table_type(::uwvm2::parser::wasm::standard::wasm1::type::table_type & table_r,
                                                        ::std::byte const* const begin,
                                                        ::std::byte const* const end,
                                                        ::uwvm2::parser::wasm::base::error_impl& err) UWVM_THROWS
    {
        // uncheck begin > end
        auto curr{begin};

        // reftype limits ...
        // unsafe (could be the section_end)
        // ^^ curr

        if(curr == end) [[unlikely]]
        {
            err.err_curr = curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::table_type_cannot_find_element;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // [reftype] limits ...
        // [ safe  ] unsafe (could be the section_end)
        // ^^ curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte elemtype{};
        ::fast_io::freestanding::my_memcpy(::std::addressof(elemtype), curr, sizeof(elemtype));

        // The element type elemtype must be funcref
        if(elemtype != 0x70) [[unlikely]]
        {
            err.err_curr = curr;
            err.err_selectable.u8 = elemtype;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::table_type_illegal_element;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        ++curr;

        // [reftype] limits ... (end)
        // [  safe ] unsafe (could be the section_end)
        //           ^^ curr

        curr = scan_limit_type(table_r.limits, curr, end, err);

        // [reftype limits ...] (end)
        // [         safe     ] unsafe (could be the section_end)
        //                      ^^ curr

        return curr;
    }

}  // namespace uwvm2::parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
