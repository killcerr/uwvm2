/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-12
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

#include <ulte/utils/macro/push_macros.h>

#ifdef UWVM_MODULE
import fast_io;
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.binfmt.binfmt_ver1;
import uwvm.io;
import uwvm.wasm.storage;
#else
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <ulte/parser/wasm/base/impl.h>
# include <ulte/parser/wasm/concepts/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
# include <ulte/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <ulte/uwvm/io/impl.h>
# include <ulte/uwvm/wasm/storage/impl.h>
#endif

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
struct Sec1
{
    inline static constexpr ::fast_io::u8string_view section_name{u8"Sec1"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id{1};

    // Expand on Sec1 here
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr bool handle_binfmt_ver1_extensible_section_define(
    ::ulte::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec1<Fs...>>>,
    [[maybe_unused]] ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
    [[maybe_unused]] ::std::byte const* section_begin,
    [[maybe_unused]] ::std::byte const* section_end,
    ::ulte::parser::wasm::base::error_impl&,
    ::std::byte const* const)
{
    ::fast_io::io::perrln(::ulte::uwvm::u8log_output, u8"test1\n");
    return true;
}

struct Feature1
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"Feature1"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    using binfmt_ver1_section_type = ::fast_io::tuple<Sec1<Fs...>>;
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
struct Sec2
{
    inline static constexpr ::fast_io::u8string_view section_name{u8"Sec2"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id{2};

    // Expand on Sec2 here
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr bool handle_binfmt_ver1_extensible_section_define(
    ::ulte::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec2<Fs...>>>,
    [[maybe_unused]] ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
    [[maybe_unused]] ::std::byte const* section_begin,
    [[maybe_unused]] ::std::byte const* section_end,
    ::ulte::parser::wasm::base::error_impl&,
    ::std::byte const* const)
{
    ::fast_io::io::perrln(::ulte::uwvm::u8log_output, u8"test2\n");

    return true;
}

struct Sec3
{
    inline static constexpr ::fast_io::u8string_view section_name{u8"Sec3"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_byte section_id{3};

    // Unexpandable section
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr bool handle_binfmt_ver1_extensible_section_define(
    ::ulte::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<Sec3>>,
    [[maybe_unused]] ::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
    [[maybe_unused]] ::std::byte const* section_begin,
    [[maybe_unused]] ::std::byte const* section_end,
    ::ulte::parser::wasm::base::error_impl&,
    ::std::byte const* const)
{
    ::fast_io::io::perrln(::ulte::uwvm::u8log_output, u8"test3\n");

    return true;
}

struct Feature2
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"Feature2"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    using binfmt_ver1_section_type = ::fast_io::tuple<Sec2<Fs...>, Sec3>;
};

struct Feature3
{
    inline static constexpr ::fast_io::u8string_view feature_name{u8"Feature3"};
    inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};
};

template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
inline constexpr auto define_wasm_binfmt_parsering_strategy(::ulte::parser::wasm::concepts::feature_reserve_type_t<Feature1>, ::fast_io::tuple<Fs...>) noexcept
{
    return ::std::addressof(::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_handle_func<Fs...>);
}

/*
root:                    wasm
                          |
                         /
                        /
binfmt:           binfmt_ver1(binfmt1)
                 /             |        \.
featurs:       Feature1     Feature2   Feature3
                 |          /      \
sections:      Sec1(ext) Sec2(ext) Sec3
*/

int main()
{
    /// @brief All feature
    constexpr ::fast_io::tuple<Feature1, Feature2, Feature3> all_features{};
    // [[maybe_unused]] using all_feature_t = decltype(all_features);

    /// @brief wasm binfmt ver1 features
    using wasm_binfmt_ver1_features_t =
        decltype(::ulte::parser::wasm::concepts::operation::get_specified_binfmt_feature_tuple_from_all_freatures_tuple<1>(all_features));
    constexpr wasm_binfmt_ver1_features_t wasm_binfmt1_features{};
    using wasm_binfmt_ver1_module_storage_t = decltype(::ulte::parser::wasm::concepts::operation::get_module_storage_type_from_tuple(wasm_binfmt1_features));
    [[maybe_unused]] constexpr auto binfmt_ver1_handler{
        ::ulte::parser::wasm::concepts::operation::get_binfmt_handler_func_p_from_tuple<1>(wasm_binfmt1_features)};

    wasm_binfmt_ver1_module_storage_t wasm_module1{};

    ::ulte::parser::wasm::base::error_impl e{};

    ::fast_io::io::perr(::ulte::uwvm::u8log_output, u8"sec1:\n");
    ::ulte::parser::wasm::binfmt::ver1::handle_all_binfmt_ver1_extensible_section(wasm_module1, 1, nullptr, nullptr, e, nullptr);
    ::fast_io::io::perr(::ulte::uwvm::u8log_output, u8"sec2:\n");
    ::ulte::parser::wasm::binfmt::ver1::handle_all_binfmt_ver1_extensible_section(wasm_module1, 2, nullptr, nullptr, e, nullptr);
    ::fast_io::io::perr(::ulte::uwvm::u8log_output, u8"sec3:\n");
    ::ulte::parser::wasm::binfmt::ver1::handle_all_binfmt_ver1_extensible_section(wasm_module1, 3, nullptr, nullptr, e, nullptr);
    ::fast_io::io::perr(::ulte::uwvm::u8log_output, u8"sec4:\n");
    try
    {
        ::ulte::parser::wasm::binfmt::ver1::handle_all_binfmt_ver1_extensible_section(wasm_module1, 4, nullptr, nullptr, e, nullptr);
    }
    catch(::fast_io::error e)
    {
        ::fast_io::perr(::ulte::uwvm::u8log_output, u8"test4, not found\n");
    }
}

/*

(stderr)

sec1:
test1

sec2:
test2

sec3:
test3

sec4:
test4, not found

*/

// macro
#include <ulte/utils/macro/pop_macros.h>
