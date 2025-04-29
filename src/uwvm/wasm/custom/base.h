/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-19
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
import ulte.parser.wasm.standard.wasm1.type;
import ulte.parser.wasm.standard.wasm1.section;
import ulte.uwvm.io;
import ulte.uwvm.utils.ansies;
import ulte.uwvm.utils.memory;
import ulte.uwvm.wasm.feature;
import ulte.uwvm.wasm.storage;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <map>  /// @todo use fast_io::string_hashmap instead
// macro
# include <utils/macro/push_macros.h>
# include <uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/string_view.h>
# include <parser/wasm/standard/wasm1/type/impl.h>
# include <parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm/io/impl.h>
# include <uwvm/utils/ansies/impl.h>
# include <uwvm/utils/memory/impl.h>
# include <uwvm/wasm/feature/impl.h>
# include <uwvm/wasm/storage/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::uwvm::wasm::custom
{
    /// @param  0: custom_begin
    /// @param  1: custom_end
    /// @return int. 0: success, others: fault
    using handlefunc_ptr_t = int (*)(parser::wasm::standard::wasm1::type::wasm_byte const*, parser::wasm::standard::wasm1::type::wasm_byte const*) noexcept;

    inline int handle_binfmt1_custom_section(::ulte::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t const& module,
                                             ::std::map<::fast_io::u8string, handlefunc_ptr_t> const& custom_handler) noexcept
    {
        for(auto& cs: module.custom_sections)
        {
            if(auto const curr_custom_handler{custom_handler.find(::fast_io::u8string{cs.custom_name})}; curr_custom_handler != custom_handler.cend())
            {
                using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = parser::wasm::standard::wasm1::type::wasm_byte const*;
                auto res{(curr_custom_handler->second)(cs.custom_begin, reinterpret_cast<wasm_byte_const_may_alias_ptr>(cs.sec_span.sec_end))};
                // 0: success, others: fault
                if(res) [[unlikely]] { return res; }
            }
        }
        return 0;
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <utils/macro/pop_macros.h>
#endif
