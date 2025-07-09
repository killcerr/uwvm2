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
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/string_view.h>
# include <uwvm2/parser/wasm_custom/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::custom::customs
{
    inline constexpr void name_handler(::uwvm2::uwvm::wasm::type::wasm_file_t & file,
                                       ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* const begin,
                                       ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* const end) noexcept
    {
        ::fast_io::vector<::uwvm2::parser::wasm_custom::customs::name_err_t> name_err{};
        ::uwvm2::parser::wasm_custom::customs::parse_name_storage(file.wasm_binfmt_ver1_name,
                                                                  reinterpret_cast<::std::byte const*>(begin),
                                                                  reinterpret_cast<::std::byte const*>(end),
                                                                  name_err);

        if(::uwvm2::uwvm::show_warning)
        {
            /// @todo name_err
        }
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
