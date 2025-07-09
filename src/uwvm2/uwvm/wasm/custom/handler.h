/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-19
 * @copyright   ASHP-1.0 License
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
import uwvm2.parser.wasm_custom;
import :cwrapper;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <map>  /// @todo use fast_io::string_hashmap instead
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/parser/wasm_custom/impl.h>
# include "cwrapper.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::custom
{
    using handlefunc_ptr_t = void (*)(::uwvm2::uwvm::wasm::type::wasm_file_t&,
                                      ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*,
                                      ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*) noexcept;

    struct handlefunc_t
    {
        void* handler{};
        bool is_imported_c{};
    };

    inline void handle_binfmtver1_custom_section(::uwvm2::uwvm::wasm::type::wasm_file_t & wasm_file,
                                                 ::std::map<::fast_io::u8string_view, handlefunc_t> const& custom_handler) noexcept
    {
        auto const& customsec{
            ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<::uwvm2::parser::wasm::standard::wasm1::features::custom_section_storage_t>(
                wasm_file.wasm_binfmt_ver1_storage.sections)};

        for(auto const& cs: customsec.customs)
        {
            if(auto const curr_custom_handler{custom_handler.find(cs.custom_name)}; curr_custom_handler != custom_handler.cend())
            {
                using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = parser::wasm::standard::wasm1::type::wasm_byte const*;

                if(curr_custom_handler->second.handler == nullptr) [[unlikely]] { continue; }

                if(curr_custom_handler->second.is_imported_c)
                {
#ifdef __cpp_exceptions
                    try
#endif
                    {
                        reinterpret_cast<::uwvm2::uwvm::wasm::custom::imported_c_handlefunc_ptr_t>(
                            curr_custom_handler->second.handler)(cs.custom_begin, reinterpret_cast<wasm_byte_const_may_alias_ptr>(cs.sec_span.sec_end));
                    }
#ifdef __cpp_exceptions
                    catch(...)
                    {
                        // An exception may be thrown to catch.
                    }
#endif
                }
                else
                {
                    reinterpret_cast<handlefunc_ptr_t>(
                        curr_custom_handler->second.handler)(wasm_file, cs.custom_begin, reinterpret_cast<wasm_byte_const_may_alias_ptr>(cs.sec_span.sec_end));
                }

                // The custom section should not be terminated if an error occurs.
            }
        }
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
