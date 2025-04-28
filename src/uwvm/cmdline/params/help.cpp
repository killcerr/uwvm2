/********************************************************
 * Ultimate WebAssembly Virtual Machine (help 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

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

#include <utils/macro/push_macros.h>
#include <uwvm/utils/ansies/uwvm_color_push_macro.h>

#ifdef UWVM_MODULE
import fast_io;
import ulte.utils.ansies;
import ulte.utils.cmdline;
import ulte.uwvm.io;
import ulte.uwvm.utils.ansies;
import ulte.uwvm.cmdline;
#else
# include <fast_io.h>
# include <fast_io_dsal/array.h>
# include <utils/ansies/impl.h>
# include <utils/cmdline/impl.h>
# include <uwvm/io/impl.h>
# include <uwvm/utils/ansies/impl.h>
# include <uwvm/cmdline/impl.h>
#endif

namespace ulte::uwvm::cmdline::paras::details
{
    template <::std::size_t N>
    inline constexpr ::fast_io::array<char8_t, N + 1> u8nspace() noexcept
    {
        ::fast_io::array<char8_t, N + 1> res{};

        for(::std::size_t i{}; i != N; ++i) { res[i] = u8' '; }

        res[N] = u8'\0';

        return res;
    }

    // max parameter + left "  " + right "  "
    inline constexpr auto parameter_max_principal_name_size_u8nspace{u8nspace<::ulte::uwvm::cmdline::parameter_max_principal_name_size + 4>()};

    inline void help_output_singal_cate(::ulte::utils::cmdline::categorization cate) noexcept
    {
        for(auto const p: ::ulte::uwvm::cmdline::parameters)
        {
            if(p->cate != cate) { continue; }
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                u8"  ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                ::fast_io::mnp::left(p->name, ::ulte::uwvm::cmdline::parameter_max_principal_name_size));
            ::fast_io::io::perrln(::ulte::uwvm::u8log_output,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  u8"  -----  ",
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  p->describe);
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                parameter_max_principal_name_size_u8nspace.element,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_PURPLE),
                                u8"Usage: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"[",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                p->name);
            for(auto curr_base{p->alias.base}; curr_base != p->alias.base + p->alias.len; ++curr_base)
            {
                ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"|",
                                    ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                    *curr_base);
            }
            ::fast_io::io::perrln(::ulte::uwvm::u8log_output,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  u8"] ",
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  p->usage,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }
    }

    inline void help_output_all() noexcept
    {
        for(auto const p: ::ulte::uwvm::cmdline::parameters)
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                u8"  ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                ::fast_io::mnp::left(p->name, ::ulte::uwvm::cmdline::parameter_max_principal_name_size));
            ::fast_io::io::perrln(::ulte::uwvm::u8log_output,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  u8"  -----  ",
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  p->describe);
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                parameter_max_principal_name_size_u8nspace.element,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_PURPLE),
                                u8"Usage: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"[",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                p->name);
            for(auto curr_base{p->alias.base}; curr_base != p->alias.base + p->alias.len; ++curr_base)
            {
                ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                    ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"|",
                                    ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                    *curr_base);
            }
            ::fast_io::io::perrln(::ulte::uwvm::u8log_output,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  u8"] ",
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  p->usage,
                                  ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }
    }

    UWVM_GNU_COLD extern ::ulte::utils::cmdline::parameter_return_type
        help_callback([[maybe_unused]] ::ulte::utils::cmdline::parameter_parsing_results* para_begin,
                      ::ulte::utils::cmdline::parameter_parsing_results* para_curr,
                      ::ulte::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        auto currp1{para_curr + 1};

        // Check for out-of-bounds and not-argument
        if(currp1 == para_end || currp1->type != ::ulte::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n");
            help_output_singal_cate(::ulte::utils::cmdline::categorization::global);

            // display other parameter
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                // ln
                                u8"\n",
                                // debug
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  ",
                                ::fast_io::mnp::left(u8"<debug>", ::ulte::uwvm::cmdline::parameter_max_principal_name_size),
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"  -----  ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Use \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"--help debug",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" to display the debug arguments."
                                // endl
                                u8"\n\n",
                                // wasm
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  ",
                                ::fast_io::mnp::left(u8"<wasm>", ::ulte::uwvm::cmdline::parameter_max_principal_name_size),
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"  -----  ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Use \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"--help wasm",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" to display the wasm arguments.",
                                // endl
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::ulte::utils::cmdline::parameter_return_type::return_imme;
        }

        currp1->type = ::ulte::utils::cmdline::parameter_parsing_results_type::occupied_arg;

        if(auto const currp1_str{currp1->str}; currp1_str == u8"all")
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n");
            help_output_all();
            ::fast_io::io::perr(::ulte::uwvm::u8log_output, ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"global")
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <global>",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(::ulte::utils::cmdline::categorization::global);
            ::fast_io::io::perr(::ulte::uwvm::u8log_output, ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"debug")
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <debug>",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(::ulte::utils::cmdline::categorization::debug);
            ::fast_io::io::perr(::ulte::uwvm::u8log_output, ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"wasm")
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <wasm>",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(::ulte::utils::cmdline::categorization::wasm);
            ::fast_io::io::perr(::ulte::uwvm::u8log_output, ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid Extra Help Name \"",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                currp1_str,
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Usage: [",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--help",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"|",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"-h",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[<null>|all|global|debug|wasm]",
                                ::fast_io::mnp::cond(::ulte::uwvm::ulte::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::ulte::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        return ::ulte::utils::cmdline::parameter_return_type::return_imme;
    }

}  // namespace ulte::uwvm::cmdline::paras::details

// macro
#include <uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <utils/macro/pop_macros.h>
