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
 
#if !(__cpp_pack_indexing >= 202311L)
# error "UWVM requires at least C++26 standard compiler."
#endif

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

struct B1F1
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B1F1"};
    inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr void binfmt_ver1_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*) UWVM_THROWS
{
    // This defines the function that handles binary format 1.
    // Supported by <::parser::wasm::concepts::wasm_feature... Fs> Continued Expansion
    []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
    { ((::fast_io::io::perrln(::utils::u8err, u8"binfmt1: ", Fs...[I] ::feature_name)), ...); }(::std::make_index_sequence<sizeof...(Fs)>{});
}

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr auto
    define_wasm_binfmt_parsering_strategy(::parser::wasm::concepts::feature_reserve_type_t<B1F1>, ::fast_io::tuple<Fs...>) noexcept
{
    // Since B1F1 defines binfmt as 1 and also defines the handler function, this function is the parsing function for binfmt1
    return ::std::addressof(binfmt_ver1_handle_func<Fs...>);
}

struct binfmt_ver1_module_storage_t
{
    // This one is used to automatically synthesize storage structures, and supports extensions
};

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr binfmt_ver1_module_storage_t define_wasm_binfmt_storage_type(::parser::wasm::concepts::feature_reserve_type_t<B1F1>,
                                                                              ::fast_io::tuple<Fs...>) noexcept
{
    return binfmt_ver1_module_storage_t{};
}

struct B1F2
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B1F2"};
    inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

struct B2F3
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B2F3"};
    inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{2u};
};

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr void binfmt_ver2_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*) UWVM_THROWS
{
    // This defines the function that handles binary format 2.
    // Supported by <::parser::wasm::concepts::wasm_feature... Fs> Continued Expansion
    []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
    { ((::fast_io::io::perrln(::utils::u8err, u8"binfmt2: ", Fs...[I] ::feature_name)), ...); }(::std::make_index_sequence<sizeof...(Fs)>{});
}

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr auto
    define_wasm_binfmt_parsering_strategy(::parser::wasm::concepts::feature_reserve_type_t<B2F3>, ::fast_io::tuple<Fs...>) noexcept
{
    // Since B2F3 defines binfmt as 2 and also defines the handler function, this function is the parsing function for binfmt1
    return ::std::addressof(binfmt_ver2_handle_func<Fs...>);
}

struct binfmt_ver2_module_storage_t
{
    // This one is used to automatically synthesize storage structures, and supports extensions
};

template <::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr binfmt_ver2_module_storage_t define_wasm_binfmt_storage_type(::parser::wasm::concepts::feature_reserve_type_t<B2F3>,
                                                                              ::fast_io::tuple<Fs...>) noexcept
{
    return binfmt_ver2_module_storage_t{};
}

int main()
{
    constexpr ::fast_io::tuple<B1F1, B1F2, B2F3> features{};

    constexpr auto binfmt1_funcp{::parser::wasm::concepts::operation::get_binfmt_handler_func_p_from_tuple<1>(features)};
    // Passed tuple structures can only be passed structures with a binfmt of 1
    binfmt_ver1_module_storage_t storage1 = binfmt1_funcp(::fast_io::tuple<B1F1, B1F2>{}, nullptr, nullptr);

    constexpr auto binfmt2_funcp{::parser::wasm::concepts::operation::get_binfmt_handler_func_p_from_tuple<2>(features)};
    // Passed tuple structures can only be passed structures with a binfmt of 2
    binfmt_ver2_module_storage_t storage2 = binfmt2_funcp(::fast_io::tuple<B2F3>{}, nullptr, nullptr);
}

/*
output: (stderr)
binfmt1: B1F1
binfmt1: B1F2
binfmt2: B2F3
*/
