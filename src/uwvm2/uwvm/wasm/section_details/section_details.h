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
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <tuple>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include <uwvm2/parser/wasm_custom/customs/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
# include <uwvm2/uwvm/wasm/type/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::wasm::section_details
{
    struct section_details_t
    {
        ::uwvm2::uwvm::wasm::type::all_module_t const* all_module_ptr{};
    };

    inline constexpr section_details_t section_details(::uwvm2::uwvm::wasm::type::all_module_t const& module_storage) noexcept
    {
        return {::std::addressof(module_storage)};
    }

    /// @brief Print the section details
    /// @throws maybe throw fast_io::error, see the implementation of the stream
    template <::std::integral char_type, typename Stm>
    inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, section_details_t>, Stm && stream, section_details_t const section_details)
    {
        if(section_details.all_module_ptr == nullptr) [[unlikely]] { ::fast_io::fast_terminate(); }

        auto const& module_storage{*section_details.all_module_ptr};

        switch(module_storage.type)
        {
            case ::uwvm2::uwvm::wasm::type::module_type_t::exec_wasm:
            {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                if(module_storage.module_storage_ptr.wf == nullptr) [[unlikely]] { ::uwvm2::utils::debug::trap_and_inform_bug_pos(); }
#endif

                switch(module_storage.module_storage_ptr.wf->binfmt_ver)
                {
                    case 1u:
                    {
                        // binfmt ver 1
                        if constexpr(::std::same_as<char_type, char>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                "======================\n" "Executed Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                "\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                "\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                "\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                "\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, wchar_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                L"======================\n" L"Executed Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                L"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                L"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                L"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                L"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char8_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                u8"======================\n" u8"Executed Module Name: ",
                                module_storage.module_storage_ptr.wf->module_name,
                                u8"\nFile Path: ",
                                module_storage.module_storage_ptr.wf->file_name,
                                u8"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                u8"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                u8"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char16_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                u"======================\n" u"Executed Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                u"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                u"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                u"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                u"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char32_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                U"======================\n" U"Executed Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                U"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                U"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                U"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                U"\n\n");
                        }

                        break;
                    }
                    default:
                    {
                        static_assert(::uwvm2::uwvm::wasm::feature::max_binfmt_version == 1u, "missing implementation of other binfmt version");
                        /// @todo Maybe I forgot to realize it.
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                        ::std::unreachable();
                    }
                }

                break;
            }
            case ::uwvm2::uwvm::wasm::type::module_type_t::preloaded_wasm:
            {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                if(module_storage.module_storage_ptr.wf == nullptr) [[unlikely]] { ::uwvm2::utils::debug::trap_and_inform_bug_pos(); }
#endif
                switch(module_storage.module_storage_ptr.wf->binfmt_ver)
                {
                    // binfmt ver 1
                    case 1u:
                    {
                        if constexpr(::std::same_as<char_type, char>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                "======================\n" "Preloaded Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                "\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                "\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                "\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                "\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, wchar_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                L"======================\n" L"Preloaded Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                L"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                L"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                L"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                L"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char8_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                u8"======================\n" u8"Preloaded Module Name: ",
                                module_storage.module_storage_ptr.wf->module_name,
                                u8"\nFile Path: ",
                                module_storage.module_storage_ptr.wf->file_name,
                                u8"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                u8"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                u8"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char16_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                u"======================\n" u"Preloaded Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                u"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                u"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                u"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                u"\n\n");
                        }
                        else if constexpr(::std::same_as<char_type, char32_t>)
                        {
                            ::fast_io::operations::print_freestanding<false>(
                                ::std::forward<Stm>(stream),
                                U"======================\n" U"Preloaded Module Name: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->module_name),
                                U"\nFile Path: ",
                                ::fast_io::mnp::code_cvt(module_storage.module_storage_ptr.wf->file_name),
                                U"\nBinary Format Version: ",
                                module_storage.module_storage_ptr.wf->binfmt_ver,
                                U"\n",
                                ::uwvm2::parser::wasm::binfmt::ver1::section_details(
                                    module_storage.module_storage_ptr.wf->get_curr_binfmt_version_wasm_storage<1u>()),
                                U"\n\n");
                        }

                        break;
                    }
                    default:
                    {
                        static_assert(::uwvm2::uwvm::wasm::feature::max_binfmt_version == 1u, "missing implementation of other binfmt version");
                        /// @todo Maybe I forgot to realize it.
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                        ::std::unreachable();
                    }
                }

                break;
            }
#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))
            case ::uwvm2::uwvm::wasm::type::module_type_t::preloaded_dl:
            {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                if(module_storage.module_storage_ptr.wd == nullptr) [[unlikely]] { ::uwvm2::utils::debug::trap_and_inform_bug_pos(); }
# endif
                /// @todo
                break;
            }
#endif
            case ::uwvm2::uwvm::wasm::type::module_type_t::local_import:
            {
                /// @todo
                break;
            }
            default:
            {
                /// @todo Maybe I forgot to realize it.
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                ::std::unreachable();
            }
        }
    }

    inline constexpr void print_section_details() noexcept
    {
        ::fast_io::basic_obuf<::fast_io::u8native_io_observer> u8out_obuf{::fast_io::u8out()};

#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"print section details"};
#endif

        ::fast_io::io::print(u8out_obuf, u8"All Module Section Details:\n\n");

        for(auto const& curr_module: ::uwvm2::uwvm::wasm::storage::all_module) { ::fast_io::io::print(u8out_obuf, section_details(curr_module.second)); }
    }

}  // namespace uwvm2::uwvm::wasm::section_details

#ifndef UWVM_MODULE
# include <uwvm2/utils/macro/pop_macros.h>
#endif
