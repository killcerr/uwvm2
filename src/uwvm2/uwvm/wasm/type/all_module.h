/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @copyright   APL-2.0 License
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

#ifndef UWVM_MODULE
// std
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <new>
# include <memory>
# include <type_traits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm_custom/customs/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
# include "para.h"
# include "file.h"
# include "dl.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::type
{
    enum class module_type_t : unsigned
    {
        exec_wasm,       // wasm_file
        preloaded_wasm,  // wasm_file
        preloaded_dl,    // wasm_dl
        local_import     /// @todo
    };

    // all module

    union module_storage_ptr_u
    {
        ::uwvm2::uwvm::wasm::type::wasm_file_t const* wf;
        /// @todo local_import

#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))
        ::uwvm2::uwvm::wasm::type::wasm_dl_t const* wd;
#endif
    };

    struct all_module_t
    {
        module_storage_ptr_u module_storage_ptr{};
        module_type_t type{};
    };

    // all module export

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline consteval auto get_binfmt_ver1_final_export_type_from_tuple(::uwvm2::utils::container::tuple<Fs...>) noexcept
    {
        return ::uwvm2::parser::wasm::standard::wasm1::features::final_export_type_t<Fs...>{};
    }

    struct wasm_file_export_t
    {
        using wasm_binfmt_ver1_export_storage_t = decltype(get_binfmt_ver1_final_export_type_from_tuple(::uwvm2::uwvm::wasm::feature::wasm_binfmt1_features));

        union wasm_file_export_storage_u
        {
            wasm_binfmt_ver1_export_storage_t const* wasm_binfmt_ver1_export_storage_ptr;
        };

        wasm_file_export_storage_u storage{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_ver{1u};

        // wasm only supported binfmt version 1 currently
        static_assert(::uwvm2::uwvm::wasm::feature::max_binfmt_version == 1u, "missing implementation of other binfmt version");
    };

    struct wasm_dl_export_t
    {
        using wasm_dl_export_storage_t = ::uwvm2::uwvm::wasm::type::capi_function_t;
        wasm_dl_export_storage_t const* storage{};
    };

    struct all_module_export_t
    {
        using wasm_file_export_storage_t = wasm_file_export_t;
        using wasm_dl_export_storage_t = wasm_dl_export_t;

        union all_module_export_storage_u
        {
            wasm_file_export_storage_t wasm_file_export_storage_ptr;
            static_assert(::std::is_trivially_copyable_v<wasm_file_export_storage_t> && ::std::is_trivially_destructible_v<wasm_file_export_storage_t>);
            wasm_dl_export_storage_t wasm_dl_export_storage_ptr;
            static_assert(::std::is_trivially_copyable_v<wasm_dl_export_storage_t> && ::std::is_trivially_destructible_v<wasm_dl_export_storage_t>);
        };

        all_module_export_storage_u storage{};

        module_type_t type{};
    };

    // map<module_name, map<export_name, all_module_export_t>>

}  // namespace uwvm2::uwvm::wasm::type

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
