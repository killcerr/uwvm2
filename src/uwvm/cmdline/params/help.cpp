/********************************************************
 * Ultimate WebAssembly Virtual Machine (help 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @help     2.0.0
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

#include <utils/macro/push_macros.h>
#include <utils/ansies/ansi_push_macro.h>

import fast_io;
import utils.io;
import utils.cmdline;
import uwvm.cmdline;

namespace uwvm::cmdline::paras::details
{
    UWVM_GNU_COLD extern ::utils::cmdline::parameter_return_type help_callback(::utils::cmdline::parameter_parsing_results*,
                                                                               ::utils::cmdline::parameter_parsing_results*,
                                                                               ::utils::cmdline::parameter_parsing_results*) noexcept
    {
        ::fast_io::io::perr(::utils::u8err, UWVM_AES_U8_RST_ALL UWVM_AES_U8_WHITE u8"Arguments:\n");
        for(auto const& p: ::uwvm::cmdline::parameters)
        {
            ::fast_io::io::perr(::utils::u8err, u8"    " UWVM_AES_U8_GREEN, p->name);
            if(p->alias.len != 0)
            {
                ::fast_io::io::perr(::utils::u8err, UWVM_AES_U8_WHITE u8", ");
                auto i{p->alias.base};
                for(; i != p->alias.base + p->alias.len - 1; ++i) { ::fast_io::io::perr(::utils::u8err, UWVM_AES_U8_GREEN, *i, UWVM_AES_U8_WHITE u8", "); }
                ::fast_io::io::perr(::utils::u8err, UWVM_AES_U8_GREEN, *i);
            }
            ::fast_io::io::perrln(::utils::u8err, UWVM_AES_U8_GRAY u8" --- " UWVM_AES_U8_WHITE, p->describe);
        }
        ::fast_io::io::perr(::utils::u8err, UWVM_AES_U8_RST_ALL u8"\n");

        return ::utils::cmdline::parameter_return_type::return_imme;
    }

}  // namespace uwvm::cmdline::paras::details
