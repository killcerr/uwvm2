/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
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
import ulte.parser.wasm.standard.wasm1.type;
import :base;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <map>  /// @todo use fast_io::string_hashmap instead
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/string_view.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include "base.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::uwvm::wasm::custom
{
    inline ::std::map<::fast_io::u8string, ::ulte::uwvm::wasm::custom::handlefunc_ptr_t>
        custom_handle_funcs{};  // [global] No global variable dependencies from other translation units

    inline void handle_singal_binfmt1_custom_section(::ulte::parser::wasm::standard::wasm1::section::custom_section const& cs) noexcept
    {
        if(auto const curr_custom_handler{custom_handle_funcs.find(::fast_io::u8string{cs.custom_name})}; curr_custom_handler != custom_handle_funcs.cend())
        {
            using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = parser::wasm::standard::wasm1::type::wasm_byte const*;
            auto res{(curr_custom_handler->second)(cs.custom_begin, reinterpret_cast<wasm_byte_const_may_alias_ptr>(cs.sec_span.sec_end))};
            if(!res) [[unlikely]]
            {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
                ::fast_io::io::perr(
                    ::ulte::uwvm::u8log_output,
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                    u8"uwvm: ",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                    u8"[error] ",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                    u8"(offset=",
                    ::fast_io::mnp::addrvw(reinterpret_cast<::std::byte const*>(cs.custom_begin) -
                                           reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin())),
                    u8") Handle Custom Section \"",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                    cs.custom_name,
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                    u8"\" Fault!\nMemory: ",
                    ::ulte::uwvm::utils::memory::print_memory{reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                                                              reinterpret_cast<::std::byte const*>(cs.custom_begin),
                                                              reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend())},
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                    u8"\n\n");
#endif
            }
        }
    }

    inline void handle_binfmt1_custom_section(::ulte::parser::wasm::standard::wasm1::section::custom_section const* custom_sections_begin,
                                              ::ulte::parser::wasm::standard::wasm1::section::custom_section const* custom_sections_end) noexcept
    {
        for(auto curr{custom_sections_begin}; curr != custom_sections_end; ++curr) { handle_singal_binfmt1_custom_section(*curr); }
    }
}

#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
