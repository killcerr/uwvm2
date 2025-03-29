/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
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

module;

#include <memory>

#include <utils/macro/push_macros.h>

export module uwvm.cmdline.params:version;

import fast_io;
import utils.io;
import utils.cmdline;

export namespace uwvm::cmdline::paras
{
    namespace details
    {
        inline bool version_is_exist{};
        inline constexpr ::fast_io::u8string_view version_alias{u8"-v"};

        UWVM_GNU_COLD inline ::utils::cmdline::parameter_return_type version_callback(::utils::cmdline::parameter_parsing_results* para_begin,
                                                                                      ::utils::cmdline::parameter_parsing_results* para_curr,
                                                                                      ::utils::cmdline::parameter_parsing_results* para_end) noexcept
        {

            ::fast_io::io::perr(::utils::u8err, u8"UWVM\n");
            return ::utils::cmdline::parameter_return_type::return_imme;
        }
    }  // namespace details

    inline constexpr ::utils::cmdline::parameter version{.name{::fast_io::u8string_view{u8"--version"}},
                                                         .describe{::fast_io::u8string_view{u8"Output version information. Usage: [--version|-v]"}},
                                                         .alias{::utils::cmdline::kns_u8_str_scatter_t{::std::addressof(details::version_alias), 1}},
                                                         .handle{::std::addressof(details::version_callback)},
                                                         .is_exist{::std::addressof(details::version_is_exist)}};
}  // namespace uwvm::cmdline::paras
