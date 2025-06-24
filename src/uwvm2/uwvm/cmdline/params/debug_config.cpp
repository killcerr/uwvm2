/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-07
 * @copyright   ASHP-1.0 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

#ifdef _DEBUG

# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <concepts>
# include <memory>

# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>

# ifdef UWVM_MODULE
import fast_io;
import uwvm2.utils.cmdline;
import uwvm2.uwvm.io;
# else
#  include <fast_io.h>
#  include <uwvm2/utils/cmdline/impl.h>
#  include <uwvm2/uwvm/io/impl.h>
# endif

namespace uwvm2::uwvm::cmdline::paras::details
{
    UWVM_GNU_COLD extern ::uwvm2::utils::cmdline::parameter_return_type
        debug_config_callback([[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_begin,
                              [[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_curr,
                              [[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
#if defined(UWVM_MODE_MINSIZEREL)
                    ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"`--debug-config` ain't available in the uwvm minsizerel mode.\n\n");
#else

#endif

        // return imme
        return ::uwvm2::utils::cmdline::parameter_return_type::return_imme;
    }

}  // namespace uwvm2::uwvm::cmdline::paras::details

// macro
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>

#endif
