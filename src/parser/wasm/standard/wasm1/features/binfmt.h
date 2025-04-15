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

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>
#include <memory>

#include <utils/macro/push_macros.h>

#ifdef UWVM_MODULE
export module parser.wasm.standard.wasm1.features:binfmt;
#endif

#ifdef UWVM_MODULE
import fast_io;
import utils.io;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
import :type_section;
#else
#include <fast_io.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/tuple.h>
#include <utils/io/impl.h>
#include <parser/wasm/concepts/impl.h>
#include <parser/wasm/standard/wasm1/type/impl.h>
#include <parser/wasm/standard/wasm1/section/impl.h>
#include <parser/wasm/standard/wasm1/opcode/impl.h>
#include <parser/wasm/binfmt/binfmt_ver1/impl.h>
#endif

#ifdef UWVM_MODULE
export
#endif
namespace parser::wasm::standard::wasm1::features
{
    struct wasm1
    {
        inline static constexpr ::fast_io::u8string_view feature_name{u8"WebAssembly Release 1.0 (2019-07-20)"};
        inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

        template <::parser::wasm::concepts::wasm_feature... Fs>
        using binfmt_ver1_section_type = ::fast_io::tuple<::parser::wasm::standard::wasm1::features::type_section_storage_t<Fs...>
                                                          /// @todo
                                                          >;
    };

    static_assert(::parser::wasm::concepts::wasm_feature<wasm1>, "struct wasm1 not match wasm feature");

    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr auto define_wasm_binfmt_parsering_strategy(::parser::wasm::concepts::feature_reserve_type_t<wasm1>, ::fast_io::tuple<Fs...>) noexcept
    {
        return ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_handle_func<Fs...>;
    }

    static_assert(::parser::wasm::concepts::has_wasm_binfmt_parsering_strategy<wasm1>, "struct wasm1 did not has define_wasm_binfmt_parsering_strategy");

}  // namespace parser::wasm::standard::wasm1::features
