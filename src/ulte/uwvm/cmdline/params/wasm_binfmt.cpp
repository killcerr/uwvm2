﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-30
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

#include <memory>

#include <ulte/utils/macro/push_macros.h>
#include <ulte/uwvm/utils/ansies/uwvm_color_push_macro.h>

#ifdef UWVM_MODULE
import fast_io;
import ulte.utils.ansies;
import ulte.utils.cmdline;
import ulte.parser.wasm.standard.wasm1.type;
import ulte.uwvm.io;
import ulte.uwvm.utils.ansies;
import ulte.uwvm.cmdline;
import ulte.uwvm.wasm.base;
import ulte.uwvm.wasm.storage;
#else
# include <fast_io.h>
# include <ulte/utils/ansies/impl.h>
# include <ulte/utils/cmdline/impl.h>
# include <ulte/parser/wasm/standard/wasm1/type/impl.h>
# include <ulte/uwvm/io/impl.h>
# include <ulte/uwvm/utils/ansies/impl.h>
# include <ulte/uwvm/cmdline/impl.h>
# include <ulte/uwvm/wasm/base/impl.h>
# include <ulte/uwvm/wasm/storage/impl.h>
#endif

namespace ulte::uwvm::cmdline::paras::details
{
    UWVM_GNU_COLD extern ::ulte::utils::cmdline::parameter_return_type
        wasm_binfmt_callback([[maybe_unused]] ::ulte::utils::cmdline::parameter_parsing_results* para_begin,
                             ::ulte::utils::cmdline::parameter_parsing_results* para_curr,
                             ::ulte::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        // "--abi" xxxxxxxxx
        //          currp1^
        auto currp1{para_curr + 1};

        // Check for out-of-bounds and not-argument
        if(currp1 == para_end || currp1->type != ::ulte::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Usage: [",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--wasm-binfmt",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"|",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"-Wbf",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[detect|1]",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");
            return ::ulte::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        // Setting the argument is already taken
        currp1->type = ::ulte::utils::cmdline::parameter_parsing_results_type::occupied_arg;

        auto const currp1_str{currp1->str};

        if(currp1_str == u8"detect")
        {
            ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver = 0;
            return ::ulte::utils::cmdline::parameter_return_type::def;
        }

        ::ulte::parser::wasm::standard::wasm1::type::wasm_u32 binfmt_version{};
        auto const [next, err]{::fast_io::parse_by_scan(currp1_str.cbegin(), currp1_str.cend(), binfmt_version)};

        // parse u32 error
        if(err != ::fast_io::parse_code::ok || next != currp1_str.cend()) [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid Binary Format Version \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                currp1_str,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Usage: [",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--wasm-binfmt",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"|",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"-Wbf",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[detect|1]",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::ulte::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        if(binfmt_version == static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(1u))
        {
            ::ulte::uwvm::wasm::storage::execute_wasm_binfmt_ver = static_cast<::ulte::parser::wasm::standard::wasm1::type::wasm_u32>(1u);
        }
        else [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid Binary Format Version \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                binfmt_version,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Usage: [",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--wasm-binfmt",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"|",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"-Wbf",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[detect|1]",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::ulte::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        return ::ulte::utils::cmdline::parameter_return_type::def;
    }

}  // namespace ulte::uwvm::cmdline::paras::details

// macro
#include <ulte/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <ulte/utils/macro/pop_macros.h>
