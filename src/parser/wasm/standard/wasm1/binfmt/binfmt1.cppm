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

module;

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>

#include <utils/macro/push_macros.h>

export module parser.wasm.standard.wasm1.binfmt:binfmt1;

import fast_io;
import utils.io;
import parser.wasm.concepts;

export namespace parser::wasm::standard::wasm1::binfmt
{

    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void wasm_binfmt_ver1_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*) UWVM_THROWS
    {
        /// @todo TODO
    }
}  // namespace parser::wasm::standard::wasm1::binfmt
