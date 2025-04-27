/********************************************************
 * Ultimate WebAssembly Virtual Machine (help 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-04-07
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

#ifdef _DEBUG

# include <cstddef>
# include <cstdint>
# include <type_traits>
# include <concepts>
# include <memory>

# include <utils/macro/push_macros.h>
# include <uwvm/utils/ansies/uwvm_color_push_macro.h>

# ifdef UWVM_MODULE
import fast_io;
import uwvm.io;
import utils.cmdline;
# else
#  include <fast_io.h>
#  include <uwvm/io/impl.h>
#  include <utils/cmdline/impl.h>
# endif

namespace uwvm::cmdline::paras::details
{
    UWVM_GNU_COLD extern ::utils::cmdline::parameter_return_type test_callback([[maybe_unused]] ::utils::cmdline::parameter_parsing_results* para_begin,
                                                                               [[maybe_unused]] ::utils::cmdline::parameter_parsing_results* para_curr,
                                                                               [[maybe_unused]] ::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        // Write the test here

        // return imme
        return ::utils::cmdline::parameter_return_type::return_imme;
    }

}  // namespace uwvm::cmdline::paras::details

// macro
# include <uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <utils/macro/pop_macros.h>

#endif
