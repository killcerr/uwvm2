/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-30
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

#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))

// std
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# ifdef UWVM_MODULE
import fast_io;
import uwvm2.utils.ansies;
import uwvm2.utils.cmdline;
import uwvm2.uwvm.io;
import uwvm2.uwvm.utils.ansies;
import uwvm2.uwvm.cmdline;
import uwvm2.uwvm.wasm.base;
import uwvm2.uwvm.wasm.storage;
import uwvm2.uwvm.wasm.loader;
# else
#  include <fast_io.h>
#  include <uwvm2/utils/ansies/impl.h>
#  include <uwvm2/utils/cmdline/impl.h>
#  include <uwvm2/uwvm/io/impl.h>
#  include <uwvm2/uwvm/utils/ansies/impl.h>
#  include <uwvm2/uwvm/cmdline/impl.h>
#  include <uwvm2/uwvm/wasm/base/impl.h>
#  include <uwvm2/uwvm/wasm/storage/impl.h>
#  include <uwvm2/uwvm/wasm/loader/impl.h>
# endif

namespace uwvm2::uwvm::cmdline::params::details
{
    UWVM_GNU_COLD extern ::uwvm2::utils::cmdline::parameter_return_type
        wasm_register_dl_callback([[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_begin,
                                  ::uwvm2::utils::cmdline::parameter_parsing_results* para_curr,
                                  ::uwvm2::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        // [... curr] ...
        // [  safe  ] unsafe (could be the module_end)
        //      ^^ para_curr

        auto currp1{para_curr + 1u};

        // [... curr] ...
        // [  safe  ] unsafe (could be the module_end)
        //            ^^ currp1

        // Check for out-of-bounds and not-argument
        if(currp1 == para_end || currp1->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            // (currp1 == para_end):
            // [... curr] ...
            // [  safe  ] unsafe (could be the module_end)
            //            ^^ currp1

            // (currp1->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg):
            // [... curr para] ...
            // [    safe     ] unsafe (could be the module_end)
            //           ^^ currp1

            ::fast_io::io::perr(::uwvm2::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Usage: ",
                                ::uwvm2::utils::cmdline::print_usage(::uwvm2::uwvm::cmdline::params::wasm_register_dl),
                                // print_usage comes with UWVM_COLOR_U8_RST_ALL
                                u8"\n\n");
            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        // [... curr arg] ...
        // [    safe    ] unsafe (could be the module_end)
        //           ^^ currp1

        // Setting the argument is already taken
        currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;

        // file name
        auto const currp1_str{currp1->str};

        ::fast_io::u8cstring_view const file_name{currp1_str};

        ::fast_io::u8string_view rename_module_name{};

        // Check for out-of-bounds and not-argument
        if(auto currp2{para_curr + 2u}; !(currp2 == para_end || currp2->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg)) [[unlikely]]
        {
            // [... curr arg arg] ...
            // [    safe        ] unsafe (could be the module_end)
            //               ^^ currp2

            // Setting the argument is already taken
            currp2->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;

            // rename
            auto const currp2_str{currp2->str};

            rename_module_name = ::fast_io::u8string_view{currp2_str};
        }

        auto& new_preloaded_dl{::uwvm2::uwvm::wasm::storage::preloaded_dl.emplace_back()};

        auto const load_dl_rtl{
            ::uwvm2::uwvm::wasm::loader::load_dl(new_preloaded_dl, file_name, rename_module_name, ::uwvm2::uwvm::wasm::storage::wasm_parameter)};

        switch(load_dl_rtl)
        {
            [[likely]] case ::uwvm2::uwvm::wasm::loader::load_dl_rtl::ok:
            {
                break;
            }
            [[unlikely]] default:
            {
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }
        }

        return ::uwvm2::utils::cmdline::parameter_return_type::def;
    }

}  // namespace uwvm2::uwvm::cmdline::params::details

// macro

# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>

#endif
