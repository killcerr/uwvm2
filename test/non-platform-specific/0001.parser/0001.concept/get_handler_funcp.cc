/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-07
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

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <concepts>
#include <memory>

#include <utils/macro/push_macros.h>

import fast_io;
import utils.io;
import parser.wasm.standard.wasm1.type;
import parser.wasm.concepts;

struct feature1
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"1"};
    inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr void binfmt_ver1_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*) UWVM_THROWS
{
    ::fast_io::io::perr(::utils::u8err, u8"test\n");
}

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr ::parser::wasm::concepts::binfmt_handle_version_func_p_type<Fs...>
    define_wasm_binfmt_parsering_strategy(::parser::wasm::concepts::feature_reserve_type_t<feature1>, ::fast_io::tuple<Fs...>) noexcept
{
    return ::std::addressof(binfmt_ver1_handle_func<Fs...>);
}

struct feature2
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"2"};
    inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

static_assert(::parser::wasm::concepts::has_wasm_binfmt_parsering_strategy<feature1>);

int main()
{
    constexpr ::fast_io::tuple<feature1, feature2> features{};
    constexpr auto funcp{::parser::wasm::concepts::operation::get_handler_func_p_from_tuple(1, features)};
    funcp(features, nullptr, nullptr);
}
