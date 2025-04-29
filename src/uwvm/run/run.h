/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 UlteSoft. All rights reserved.    *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-27
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
import ulte.uwvm.io;
import ulte.utils.ansies;
# ifdef UWVM_TIMER
import ulte.utils.debug;
# endif
import ulte.utils.madvise;
import ulte.parser.wasm.base;
import ulte.parser.wasm.concepts;
import ulte.parser.wasm.standard;
import ulte.parser.wasm.binfmt.base;
import ulte.uwvm.utils.ansies;
import ulte.uwvm.utils.memory;
import ulte.uwvm.cmdline;
import ulte.uwvm.wasm.base;
import ulte.uwvm.wasm.storage;
import ulte.uwvm.wasm.feature;
import ulte.uwvm.wasm.custom;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
// macro
# include <utils/macro/push_macros.h>
# include <uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm/io/impl.h>
# include <utils/ansies/impl.h>
# ifdef UWVM_TIMER
#  include <utils/debug/impl.h>
# endif
# include <utils/madvise/impl.h>
# include <parser/wasm/base/impl.h>
# include <parser/wasm/concepts/impl.h>
# include <parser/wasm/standard/impl.h>
# include <parser/wasm/binfmt/base/impl.h>
# include <uwvm/utils/ansies/impl.h>
# include <uwvm/utils/memory/impl.h>
# include <uwvm/cmdline/impl.h>
# include <uwvm/wasm/base/impl.h>
# include <uwvm/wasm/storage/impl.h>
# include <uwvm/wasm/feature/impl.h>
# include <uwvm/wasm/custom/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif
UWVM_MODULE_EXPORT namespace ulte::uwvm::run
{
    inline int run() noexcept
    {
        if(!::ulte::uwvm::cmdline::wasm_file_ppos) [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"No input file.\nTry \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--help",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" for more information.\n\n",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
            return -2;  // The specified file is not available or cannot be opened
        }

        auto module_name{::ulte::uwvm::cmdline::wasm_file_ppos->str};

#ifdef __cpp_exceptions
        try
#endif
        {
#ifdef UWVM_TIMER
            ::ulte::utils::debug::timer parsing_timer{u8"file loader"};
#endif
            ::ulte::uwvm::wasm::storage::execute_wasm_file = ::fast_io::native_file_loader{module_name};
        }
#ifdef __cpp_exceptions
        catch(::fast_io::error e)
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Unable to open WASM file \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                module_name,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\": ",
                                e,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n"
# ifndef _WIN32  // Win32 automatically adds a newline (winnt and win9x)
                                u8"\n"
# endif
            );
            return -2;  // The specified file is not available or cannot be opened
        }
#endif

        // If not specified by the user, detect it
        if(::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver == 0u)
        {
            ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver = ::ulte::parser::wasm::binfmt::detect_wasm_binfmt_version(
                reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend()));
        }

        // After detect
        // Instructs to read the file all the way into memory
        ::ulte::utils::madvise::my_madvise(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin(),
                                           ::ulte::uwvm::wasm::storage::execute_wasm_file.size(),
                                           ::ulte::utils::madvise::madvise_flag::willneed);

        switch(::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver)
        {
            case 0u:
            {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
                ::fast_io::io::perr(
                    ::ulte::uwvm::u8log_output,
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                    u8"uwvm: ",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                    u8"[error] ",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                    u8"(offset=",
                    ::fast_io::mnp::addrvw(nullptr),
                    u8") Illegal WebAssembly file format.",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                    u8"\nparser: ",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                    u8"[curr]",
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                    u8" -> ",
                    ::ulte::uwvm::utils::memory::print_memory{reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                                                              reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                                                              reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend())},
                    ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                    u8"\n\n");
#endif
                return -3;  // wasm parsing error
            }
            case 1u:
            {
                // handle exec module
                {
                    // set module name
                    ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver1_storage.module_name = ::fast_io::u8string_view{module_name};

                    // storage wasm err
                    ::ulte::parser::wasm::base::error_impl execute_wasm_binfmt_ver1_storage_wasm_err{};

                    // parse wasm 1
#if defined(__cpp_exceptions) && !defined(UWVM_TERMINATE_IMME_WHEN_PARSE)
                    try
#endif
                    {
#ifdef UWVM_TIMER
                        ::ulte::utils::debug::timer parsing_timer{u8"parse binfmt ver1"};
#endif
                        ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver1_storage = ::ulte::uwvm::wasm::feature::binfmt_ver1_handler(
                            ::ulte::uwvm::wasm::feature::wasm_binfmt1_features,
                            reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                            reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend()),
                            execute_wasm_binfmt_ver1_storage_wasm_err);
                    }
#if defined(__cpp_exceptions) && !defined(UWVM_TERMINATE_IMME_WHEN_PARSE)
                    catch(::fast_io::error e)
                    {
# ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
                        if(execute_wasm_binfmt_ver1_storage_wasm_err.err_code != ::ulte::parser::wasm::base::wasm_parse_error_code::ok) [[likely]]
                        {
                            // print error
                            ::ulte::parser::wasm::base::error_output_t errout;
                            errout.module_begin = reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin());
                            errout.err = execute_wasm_binfmt_ver1_storage_wasm_err;
                            errout.flag.enable_ansi = static_cast<::std::uint_least8_t>(::ulte::uwvm::utils::ansies::put_color);
#  if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))
                            errout.flag.win32_use_text_attr = static_cast<::std::uint_least8_t>(!::ulte::uwvm::utils::ansies::log_win32_use_ansi_b);
#  endif
                            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                                errout,
                                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8"\nparser: ",
                                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                                u8"[curr]",
                                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8" -> ",
                                                ::ulte::uwvm::utils::memory::print_memory{
                                                    reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                                                    execute_wasm_binfmt_ver1_storage_wasm_err.err_curr,
                                                    reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend())},
                                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                                u8"\n\n");
                        }
# endif
                        return -3;  // wasm parsing error
                    }
#endif
                    // handle custom
                    auto custom_res{::ulte::uwvm::wasm::custom::handle_binfmt1_custom_section(::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver1_storage,
                                                                                              ::ulte::uwvm::wasm::custom::custom_handle_funcs)};
                    // 0: success, others: fault
                    if(custom_res) [[unlikely]]
                    {
                        return -4;  // custom handling error
                    }
                }

                /// @todo handle import modules

                break;
            }
            default: [[unlikely]] {
#ifndef UWVM_DISABLE_OUTPUT_WHEN_PARSE
                    ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                        u8"[error] ",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"(offset=",
                                        ::fast_io::mnp::addrvw(4uz),
                                        u8") Unknown Binary Format Version of WebAssembly: \"",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver,
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\".",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\nparser: ",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                        u8"[curr]",
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8" -> ",
                                        ::ulte::uwvm::utils::memory::print_memory{
                                            reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()),
                                            reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cbegin()) + 4uz,
                                            reinterpret_cast<::std::byte const*>(::ulte::uwvm::wasm::storage::execute_wasm_file.cend())},
                                        ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                        u8"\n\n");
#endif
                    return -3;  // wasm parsing error
                }
        }

        // run vm
        switch(::ulte::uwvm::wasm::storage::execute_wasm_mode)
        {
            case ::ulte::uwvm::wasm::base::mode::objdump:
            {
                /// @todo objdump
                break;
            }
            default: ::fast_io::unreachable();
        }

        return 0;
    }
}  // namespace ulte::uwvm::run

#ifndef UWVM_MODULE
// macro
# include <uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <utils/macro/pop_macros.h>
#endif
