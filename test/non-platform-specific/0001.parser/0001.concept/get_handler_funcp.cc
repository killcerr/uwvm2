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

#include <ulte/utils/macro/push_macros.h>

#ifdef UWVM_MODULE
import fast_io;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import uwvm.io;
#else
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <ulte/parser/wasm/concepts/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
# include <ulte/uwvm/io/impl.h>
#endif

struct B1F1
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B1F1"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

// For example, the common structure
struct binfmt_ver1_module_storage_t
{
    // This one is used to automatically synthesize storage structures, and supports extensions
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr binfmt_ver1_module_storage_t
    binfmt_ver1_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*, ::ulte::parser::wasm::base::error_impl&) UWVM_THROWS
{
    // This defines the function that handles binary format 1.
    // Supported by <::ulte::parser::wasm::concepts::wasm_feature... Fs> Continued Expansion
    []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
    { ((::fast_io::io::perrln(::ulte::uwvm::u8log_output, u8"binfmt1: ", Fs...[I] ::feature_name)), ...); }(::std::make_index_sequence<sizeof...(Fs)>{});
    return {};
}

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr auto define_wasm_binfmt_parsering_strategy(::ulte::parser::wasm::concepts::feature_reserve_type_t<B1F1>, ::fast_io::tuple<Fs...>) noexcept
{
    // Since B1F1 defines binfmt as 1 and also defines the handler function, this function is the parsing function for binfmt1
    return ::std::addressof(binfmt_ver1_handle_func<Fs...>);
}

struct B1F2
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B1F2"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

struct B2F3
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"B2F3"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{2u};
};

// For example, the variable parameter
template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
struct binfmt_ver2_module_storage_t
{
    // This one is used to automatically synthesize storage structures, and supports extensions
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr binfmt_ver2_module_storage_t<Fs...>
    binfmt_ver2_handle_func(::fast_io::tuple<Fs...>, ::std::byte const*, ::std::byte const*, ::ulte::parser::wasm::base::error_impl&) UWVM_THROWS
{
    // This defines the function that handles binary format 2.
    // Supported by <::ulte::parser::wasm::concepts::wasm_feature... Fs> Continued Expansion
    []<::std::size_t... I>(::std::index_sequence<I...>) constexpr noexcept
    { ((::fast_io::io::perrln(::ulte::uwvm::u8log_output, u8"binfmt2: ", Fs...[I] ::feature_name)), ...); }(::std::make_index_sequence<sizeof...(Fs)>{});
    return {};
}

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr auto define_wasm_binfmt_parsering_strategy(::ulte::parser::wasm::concepts::feature_reserve_type_t<B2F3>, ::fast_io::tuple<Fs...>) noexcept
{
    // Since B2F3 defines binfmt as 2 and also defines the handler function, this function is the parsing function for binfmt1
    return ::std::addressof(binfmt_ver2_handle_func<Fs...>);
}

int main()
{
    constexpr ::fast_io::tuple<B1F1, B1F2, B2F3> features{};

    ::ulte::parser::wasm::base::error_impl e{};

    constexpr auto binfmt1_funcp{::ulte::parser::wasm::concepts::operation::get_binfmt_handler_func_p_from_tuple<1>(features)};
    using wasm_binfmt1_features_t = decltype(::ulte::parser::wasm::concepts::operation::get_specified_binfmt_feature_tuple_from_all_freatures_tuple<1>(features));
    static_assert(::std::same_as<wasm_binfmt1_features_t, ::fast_io::tuple<B1F1, B1F2>>, "wasm_binfmt1_features_t includes only features with a binfmt of 1");
    using wasm_binfmt1_storage_t = decltype(::ulte::parser::wasm::concepts::operation::get_module_storage_type_from_tuple(wasm_binfmt1_features_t{}));
    static_assert(::std::same_as<wasm_binfmt1_storage_t, binfmt_ver1_module_storage_t>,
                  "wasm_binfmt1_storage_t is the type returned by binfmt_ver1_handle_func<B1F1, B1F2>");
    [[maybe_unused]] wasm_binfmt1_storage_t storage1 = binfmt1_funcp(wasm_binfmt1_features_t{}, nullptr, nullptr, e);

    constexpr auto binfmt2_funcp{::ulte::parser::wasm::concepts::operation::get_binfmt_handler_func_p_from_tuple<2>(features)};
    using wasm_binfmt2_features_t = decltype(::ulte::parser::wasm::concepts::operation::get_specified_binfmt_feature_tuple_from_all_freatures_tuple<2>(features));
    static_assert(::std::same_as<wasm_binfmt2_features_t, ::fast_io::tuple<B2F3>>, "wasm_binfmt2_features_t includes only features with a binfmt of 2");
    using wasm_binfmt2_storage_t = decltype(::ulte::parser::wasm::concepts::operation::get_module_storage_type_from_tuple(wasm_binfmt2_features_t{}));
    static_assert(::std::same_as<wasm_binfmt2_storage_t, binfmt_ver2_module_storage_t<B2F3>>,
                  "wasm_binfmt2_storage_t is the type of binfmt_ver2_handle_func<B2F3> return");
    [[maybe_unused]] wasm_binfmt2_storage_t storage2 = binfmt2_funcp(wasm_binfmt2_features_t{}, nullptr, nullptr, e);
}

/*
output: (stderr)
binfmt1: B1F1
binfmt1: B1F2
binfmt2: B2F3
*/

// macro
#include <ulte/utils/macro/pop_macros.h>
