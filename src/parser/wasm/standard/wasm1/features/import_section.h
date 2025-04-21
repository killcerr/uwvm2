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
import utils.ansies;
# ifdef UWVM_TIMER
import utils.debug;
# endif
import parser.wasm.base;
import parser.wasm.concepts;
import parser.wasm.standard.wasm1.type;
import parser.wasm.standard.wasm1.section;
import parser.wasm.standard.wasm1.opcode;
import parser.wasm.binfmt.binfmt_ver1;
import :def;
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
# include <utils/ansies/ansi_push_macro.h>
// import
# include <fast_io.h>
# include <utils/io/impl.h>
# include <utils/ansies/impl.h>
# ifdef UWVM_TIMER
#  include <utils/debug/impl.h>
# endif
# include <parser/wasm/base/impl.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/standard/wasm1/opcode/impl.h>
# include <parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace parser::wasm::standard::wasm1::features
{
    template <::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t
    {
        inline static constexpr ::parser::wasm::standard::wasm1::type::wasm_u32 section_id{
            static_cast<::parser::wasm::standard::wasm1::type::wasm_u32>(::parser::wasm::standard::wasm1::section::section_id::import_sec)};

        ::parser::wasm::standard::wasm1::section::section_span_view sec_span{};
    };

    /// @brief Define the handler function for type_section
    template <::parser::wasm::concepts::wasm_feature... Fs>
    inline void handle_binfmt_ver1_extensible_section_define(
        [[maybe_unused]] ::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        [[maybe_unused]] ::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        [[maybe_unused]] ::std::byte const* const section_begin,
        [[maybe_unused]] ::std::byte const* const section_end) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::utils::debug::timer parsing_timer{u8"parse import section (id: 2)"};
#endif
        /// @todo
        ::fast_io::io::perr(::utils::log_output, u8"Not Finish\n");
        ::fast_io::fast_terminate();
    }
}  // namespace parser::wasm::standard::wasm1::features

#ifndef UWVM_MODULE
// macro
# include <utils/ansies/ansi_pop_macro.h>
# include <utils/macro/pop_macros.h>
#endif
