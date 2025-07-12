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
import uwvm2.uwvm.io;
import uwvm2.utils.ansies;
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
import uwvm2.utils.debug;
# endif
import uwvm2.utils.madvise;
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard;
import uwvm2.parser.wasm.binfmt.base;
import uwvm2.uwvm.utils.ansies;
import uwvm2.uwvm.utils.memory;
import uwvm2.uwvm.cmdline;
import uwvm2.uwvm.wasm.base;
import uwvm2.uwvm.wasm.type;
import uwvm2.uwvm.wasm.storage;
import uwvm2.uwvm.wasm.feature;
import uwvm2.uwvm.wasm.custom;
#else
// std
# include <cstddef>
# include <cstdint>
# include <type_traits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/utils/ansies/impl.h>
# if defined(UWVM_TIMER) || ((defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK))
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/utils/madvise/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/impl.h>
# include <uwvm2/parser/wasm/binfmt/base/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/utils/memory/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/wasm/base/impl.h>
# include <uwvm2/uwvm/wasm/type/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
# include <uwvm2/uwvm/wasm/feature/impl.h>
# include <uwvm2/uwvm/wasm/custom/impl.h>
#endif

namespace uwvm2::uwvm::wasm::loader
{
    enum class load_dl_rtl
    {
        ok,
        load_error
    };

    /// @todo load dl not finished
    inline constexpr load_dl_rtl load_dl(::uwvm2::uwvm::wasm::type::wasm_dl_t& wd,
                                         ::fast_io::u8cstring_view load_file_name,
                                         ::fast_io::u8string_view rename_module_name,
                                         ::uwvm2::uwvm::wasm::type::wasm_parameter_u para) noexcept
    {
        wd.file_name = load_file_name;

        wd.wasm_parameter = para;

#ifdef __cpp_exceptions
        try
#endif
        {
            // verbose
            if(::uwvm2::uwvm::show_verbose) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    u8"[info]  ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Loading DL File \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    load_file_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\". ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                    u8"(verbose)\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
            }

#ifdef UWVM_TIMER
            ::uwvm2::utils::debug::timer parsing_timer{u8"dl loader"};
#endif

            wd.import_dll_file = ::fast_io::native_dll_file{load_file_name, ::fast_io::dll_mode::posix_rtld_lazy};
        }
#ifdef __cpp_exceptions
        catch(::fast_io::error e)
        {
            ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Unable to open DL file \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                load_file_name,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\": ",
                                e,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n"
# ifndef _WIN32  // Win32 automatically adds a newline (winnt and win9x)
                                u8"\n"
# endif
            );

            return load_dl_rtl::load_error;
        }
#endif

        // 1st: para --wasm-set-main-module-name
        // 2st: custom section "name": module name
        // 3st: file path

        if(rename_module_name.empty()) { wd.module_name = ::fast_io::u8string_view{load_file_name}; }
        else
        {
            wd.module_name = rename_module_name;
        }

        // verbose
        if(::uwvm2::uwvm::show_verbose) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[info]  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Overall WebAssembly Module Name \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                wd.module_name,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                u8"(verbose)\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }

        return load_dl_rtl::ok;
    }

}  // namespace uwvm2::uwvm::wasm::loader

#ifndef UWVM_MODULE
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif
