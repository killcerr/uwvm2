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
#include <memory>

#include <utils/macro/push_macros.h>

export module parser.wasm.standard.wasm1.features:binfmt;

import fast_io;
import utils.io;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.standard.wasm1.binfmt;

export namespace parser::wasm::standard::wasm1::features
{
    /// @brief
    struct wasm1
    {
        inline static constexpr ::fast_io::u8string_view feature_name{u8"WebAssembly Release 1.0 (2019-07-20)"};
        inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
    };

    static_assert(::parser::wasm::concepts::wasm_feature<wasm1>, "struct wasm1 not match wasm feature");

    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto define_wasm_binfmt_parsering_strategy(::parser::wasm::concepts::feature_reserve_type_t<wasm1>, ::fast_io::tuple<Fs...>) noexcept
    {
        return ::std::addressof(::parser::wasm::standard::wasm1::binfmt::wasm_binfmt_ver1_handle_func<Fs...>);
    }

    static_assert(::parser::wasm::concepts::has_wasm_binfmt_parsering_strategy<wasm1>, "struct wasm1 did not has define_wasm_binfmt_parsering_strategy");

}  // namespace parser::wasm::standard::wasm1::features
