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

#pragma once

#ifdef UWVM_MODULE
import fast_io;
import utils.io;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <utils/io/impl.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/standard/wasm1/opcode/impl.h>
# include <parser/wasm/binfmt/binfmt_ver1/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::standard::wasm1::features
{
    template <typename FeatureType>
    concept has_value_type = requires {
        typename FeatureType::value_type;
        ::parser::wasm::concepts::operation::details::check_is_type_replacer<::parser::wasm::concepts::operation::type_replacer,
                                                                             typename FeatureType::value_type>;
    };

    template <::parser::wasm::concepts::wasm_feature... Fs>
    using final_value_type_t = ::parser::wasm::concepts::operation::replacement_structure_t<
        ::std::conditional_t<has_value_type<Fs>, typename Fs::value_type, ::parser::wasm::concepts::operation::irreplaceable_t>...>;

    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct vec_value_type
    {
        static_assert(::std::is_enum_v<final_value_type_t<Fs...>>);

        final_value_type_t<Fs...> const* begin{};
        final_value_type_t<Fs...> const* end{};
    };

    template <::parser::wasm::concepts::wasm_feature... Fs>
    using final_result_type = vec_value_type<Fs...>;

    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct final_function_type
    {
        final_result_type<Fs...> parameter{};
        final_result_type<Fs...> result{};
    };

}  // namespace parser::wasm::standard::wasm1::features

UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::parser::wasm::standard::wasm1::features::final_function_type<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}
