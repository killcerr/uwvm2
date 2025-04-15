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

#if UWVM_MODULE
import fast_io;
import utils.io;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
#else
// std
#include <cstddef>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <utility>
#include <memory>
// macro
#include <utils/macro/push_macros.h>
#include <utils/ansies/ansi_push_macro.h>
// import
#include <fast_io.h>
#include <utils/io/impl.h>
#include <parser/wasm/concepts/impl.h>
#include <parser/wasm/standard/wasm1/type/impl.h>
#include <parser/wasm/standard/wasm1/section/impl.h>
#include <parser/wasm/standard/wasm1/opcode/impl.h>
#include <parser/wasm/binfmt/binfmt_ver1/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
#define UWVM_MODULE_EXPORT 
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::standard::wasm1::features
{
    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct type_section_storage_t
    {
        inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 section_id{1};

        ::parser::wasm::standard::wasm1::section::type_section typesec{};
    };

    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr bool
        handle_binfmt_ver1_extensible_section_define(::parser::wasm::concepts::feature_reserve_type_t<::std::remove_cvref_t<type_section_storage_t<Fs...>>>,
                                                     ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                                     [[maybe_unused]]::std::byte const* section_begin,
                                                     [[maybe_unused]]::std::byte const* section_end)
    {
        auto& type_section_storage{::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        [[maybe_unused]] auto& typesec{type_section_storage.typesec};

        /// @todo

        return true;
    }
}  // namespace parser::wasm::standard::wasm1::features
