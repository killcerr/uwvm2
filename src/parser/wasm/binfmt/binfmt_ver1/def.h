/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
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
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.parser.wasm.binfmt.base;
import :section;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
// macro
# include <utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <parser/wasm/binfmt/base/impl.h>
# include "section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::binfmt::ver1
{
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct wasm_binfmt_ver1_module_extensible_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{1u};

        ::ulte::parser::wasm::binfmt::module_span_t module_span{};
        ::fast_io::u8string_view module_name{};

        ::fast_io::vector<::ulte::parser::wasm::standard::wasm1::section::custom_section> custom_sections{};

        ::ulte::parser::wasm::binfmt::ver1::splice_section_storage_structure_t<Fs...> sections{};
    };

}  // namespace ulte::parser::wasm::binfmt::ver1

UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::ulte::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::ulte::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}
#ifndef UWVM_MODULE
// macro
# include <utils/macro/pop_macros.h>
#endif
