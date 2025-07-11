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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm_custom.customs;
import uwvm2.uwvm.wasm.base;
import uwvm2.uwvm.wasm.feature;
import :para;
#else
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
# include <fast_io_dsal/string_view.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm_custom/customs/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
# include "para.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::type
{
    struct wasm_file_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::std::size_t sizeof_wasm_file_module_storage_u{sizeof(::uwvm2::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t)};

        union wasm_file_module_storage_u UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
        {
            // This type can be initialized with all zeros
            ::uwvm2::uwvm::wasm::feature::wasm_binfmt_ver1_module_storage_t wasm_binfmt_ver1_storage;
            static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<decltype(wasm_binfmt_ver1_storage)> &&
                          ::fast_io::freestanding::is_zero_default_constructible_v<decltype(wasm_binfmt_ver1_storage)>);

            // Full occupancy is used to initialize the union, set the union to all zero.
            [[maybe_unused]] ::std::byte wasm_file_module_storage_u_reserve[sizeof_wasm_file_module_storage_u]{};

            // destructor of 'wasm_file_module_storage_u' is implicitly deleted because variant field 'typeidx_u8_vector' has a non-trivial destructor
            inline constexpr ~wasm_file_module_storage_u() {}

            // The release of wasm_file_module_storage_u is managed by struct wasm1_element_t, there is no issue of raii resources being unreleased.
        };

        // wasm file name
        ::fast_io::u8cstring_view file_name{};
        // Accurate module names that must work
        ::fast_io::u8string_view module_name{};
        // Recording the binfmt version of the wasm module
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_ver{};
        // Memory-mapped or memory-copy (for platforms that don't support memory mapping) open wasm files
        ::fast_io::native_file_loader wasm_file{};
        // Module parsing results
        wasm_file_module_storage_u wasm_module_storage{};
        // wasm_parameter_u
        ::uwvm2::uwvm::wasm::type::wasm_parameter_u wasm_parameter{};
        // (Optional) Module name + symbol name
        ::uwvm2::parser::wasm_custom::customs::name_storage_t wasm_custom_name{};

        // Since the default is to initialize to binfmt 0, there is no need to do any constructs
        inline constexpr wasm_file_t() noexcept = default;

        inline constexpr wasm_file_t(wasm_file_t const& other) noexcept = delete;

        inline constexpr wasm_file_t(wasm_file_t&& other) noexcept :
            file_name{::std::move(other.file_name)}, module_name{::std::move(other.module_name)}, binfmt_ver{::std::move(other.binfmt_ver)},
            wasm_file{::std::move(other.wasm_file)}, wasm_parameter{::std::move(other.wasm_parameter)}, wasm_custom_name{::std::move(other.wasm_custom_name)}
        {
            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::new(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage)) decltype(this->wasm_module_storage.wasm_binfmt_ver1_storage){
                        ::std::move(other.wasm_module_storage.wasm_binfmt_ver1_storage)};
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }
        }

        inline constexpr wasm_file_t& operator= (wasm_file_t const& other) noexcept = delete;

        inline constexpr wasm_file_t& operator= (wasm_file_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::std::destroy_at(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage));
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }

            this->file_name = ::std::move(other.file_name);
            this->module_name = ::std::move(other.module_name);
            this->binfmt_ver = ::std::move(other.binfmt_ver);
            this->wasm_file = ::std::move(other.wasm_file);
            this->wasm_parameter = ::std::move(other.wasm_parameter);
            this->wasm_custom_name = ::std::move(other.wasm_custom_name);

            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::new(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage)) decltype(this->wasm_module_storage.wasm_binfmt_ver1_storage){
                        ::std::move(other.wasm_module_storage.wasm_binfmt_ver1_storage)};
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }

            return *this;
        }

        inline constexpr ~wasm_file_t()
        {
            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::std::destroy_at(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage));
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }
        }

        inline constexpr void change_binfmt_ver(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 ver) noexcept
        {
            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::std::destroy_at(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage));
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }

            this->binfmt_ver = ver;

            switch(this->binfmt_ver)
            {
                case 1u:
                {
                    ::new(::std::addressof(this->wasm_module_storage.wasm_binfmt_ver1_storage)) decltype(this->wasm_module_storage.wasm_binfmt_ver1_storage){};
                    break;
                }
                [[unlikely]] default:
                {
                    break;
                }
            }
        }
    };
}  // namespace uwvm2::uwvm::wasm::storage

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
