/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-10-01
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

// std
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <utility>
// macro
#include <uwvm2/utils/macro/push_macros.h>
#include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
#ifndef UWVM_MODULE
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/ansies/impl.h>
# include <uwvm2/utils/cmdline/impl.h>
# include <uwvm2/imported/wasi/wasip1/mount_root/impl.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/cmdline/params/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
#endif

namespace uwvm2::uwvm::cmdline::params::details
{
    UWVM_GNU_COLD extern void wasi_mount_root_pretreatment(char8_t const* const*& argv_curr,
                                                           char8_t const* const* argv_end,
                                                           ::uwvm2::utils::container::vector<::uwvm2::utils::cmdline::parameter_parsing_results>& pr) noexcept
    {
        // argv_curr points to --wasi-mount-root or -Iroot
        // We need to process: dir -add ... -rm ... until we hit another parameter

        // Pre-scan to reserve exact additional capacity for pr before using *_unchecked
        {
            ::std::size_t entries_to_add{};  // not include parameter itself
            auto scan_ptr{argv_curr + 1u};
            if(scan_ptr != argv_end)
            {
                ::uwvm2::utils::container::u8cstring_view first_arg_view{::fast_io::mnp::os_c_str(*scan_ptr)};
                if(!first_arg_view.empty() && first_arg_view.front_unchecked() != u8'-')
                {
                    ++entries_to_add;  // root dir
                    ++scan_ptr;

                    bool stop_scanning{};
                    while(scan_ptr != argv_end && !stop_scanning)
                    {
                        ::uwvm2::utils::container::u8cstring_view token_view{::fast_io::mnp::os_c_str(*scan_ptr)};

                        if(token_view.empty()) [[unlikely]]
                        {
                            ++scan_ptr;
                            continue;
                        }

                        if(token_view == u8"-add" || token_view == u8"-rm" || token_view == u8"--symlink-escape-nonwasi")
                        {
                            ++entries_to_add;  // the option itself
                            ++scan_ptr;

                            while(scan_ptr != argv_end)
                            {
                                ::uwvm2::utils::container::u8cstring_view pattern_view{::fast_io::mnp::os_c_str(*scan_ptr)};

                                if(pattern_view.empty()) [[unlikely]]
                                {
                                    ++scan_ptr;
                                    continue;
                                }

                                if(pattern_view.front_unchecked() == u8'-')
                                {
                                    if(pattern_view == u8"-add" || pattern_view == u8"-rm" || pattern_view == u8"--symlink-escape-nonwasi") { break; }
                                    else
                                    {
                                        stop_scanning = true;
                                        break;
                                    }
                                }

                                ++entries_to_add;  // a pattern
                                ++scan_ptr;
                            }

                            continue;
                        }
                        else if(token_view.front_unchecked() == u8'-')
                        {
                            break;  // another parameter reached
                        }
                        else
                        {
                            ++entries_to_add;  // unexpected non-option argument after patterns
                            ++scan_ptr;
                        }
                    }
                }
            }

            if(entries_to_add) { pr.reserve(pr.size() + entries_to_add); }
        }

        // You should not add its own parameter into pr, because it has already been handled in the command-line parser!

        ++argv_curr;
        if(argv_curr == argv_end) { return; }

        // First argument must be the root directory (not starting with -)
        ::uwvm2::utils::container::u8cstring_view curr_view{::fast_io::mnp::os_c_str(*argv_curr)};

        if(curr_view.empty() || curr_view.front_unchecked() == u8'-') [[unlikely]]
        {
            // No directory specified or starts with -, let callback handle the error
            return;
        }

        // Add the root directory as an argument
        pr.emplace_back_unchecked(curr_view, nullptr, ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg);
        ++argv_curr;

        // Process -add, -rm and --symlink-escape-nonwasi options
        while(argv_curr != argv_end)
        {
            curr_view = ::uwvm2::utils::container::u8cstring_view{::fast_io::mnp::os_c_str(*argv_curr)};

            if(curr_view.empty()) [[unlikely]]
            {
                ++argv_curr;
                continue;
            }

            // Check if this is -add, -rm or --symlink-escape-nonwasi
            if(curr_view == u8"-add" || curr_view == u8"-rm" || curr_view == u8"--symlink-escape-nonwasi")
            {
                // Mark as argument (special handling)
                pr.emplace_back_unchecked(curr_view, nullptr, ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg);
                ++argv_curr;

                // Collect patterns until next -add, -rm, or other parameter
                while(argv_curr != argv_end)
                {
                    curr_view = ::uwvm2::utils::container::u8cstring_view{::fast_io::mnp::os_c_str(*argv_curr)};

                    if(curr_view.empty()) [[unlikely]]
                    {
                        ++argv_curr;
                        continue;
                    }

                    // Stop if we hit -add, -rm, --symlink-escape-nonwasi, or another parameter
                    if(curr_view.front_unchecked() == u8'-')
                    {
                        if(curr_view == u8"-add" || curr_view == u8"-rm" || curr_view == u8"--symlink-escape-nonwasi") { break; }
                        else
                        {
                            // Another parameter, we're done
                            return;
                        }
                    }

                    // Add pattern as argument
                    pr.emplace_back_unchecked(curr_view, nullptr, ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg);
                    ++argv_curr;
                }
            }
            else if(curr_view.front_unchecked() == u8'-')
            {
                // Hit another parameter, we're done
                return;
            }
            else
            {
                // Unexpected non-option argument after patterns
                pr.emplace_back_unchecked(curr_view, nullptr, ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg);
                ++argv_curr;
            }
        }
    }

    UWVM_GNU_COLD extern ::uwvm2::utils::cmdline::parameter_return_type
        wasi_mount_root_callback([[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_begin,
                                 ::uwvm2::utils::cmdline::parameter_parsing_results* para_curr,
                                 ::uwvm2::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        auto param_cursor{para_curr + 1};

        // Check for root directory argument
        if(param_cursor == para_end || param_cursor->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Usage: ",
                                ::uwvm2::utils::cmdline::print_usage(::uwvm2::uwvm::cmdline::params::wasi_mount_root),
                                u8"\n\n");

            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        ::uwvm2::imported::wasi::wasip1::mount_root::mount_root_entry entry{};
        entry.root_dir = ::fast_io::u8string_view{param_cursor->str};
        param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
        ++param_cursor;

        // Process -add, -rm and --symlink-escape-nonwasi options
        enum class mode_type : unsigned
        {
            none,
            add_mode,
            rm_mode,
            symlink_escape_mode
        };

        mode_type current_mode{mode_type::none};

        // Pre-scan to reserve capacities for add/rm/symlink_escape patterns and validate structure
        ::std::size_t add_count{};
        ::std::size_t rm_count{};
        ::std::size_t symlink_escape_count{};

        {
            auto scan_ptr{param_cursor};
            mode_type scan_mode{mode_type::none};
            bool expecting_pattern{false};

            while(scan_ptr != para_end && scan_ptr->type == ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg)
            {
                auto const token_str{scan_ptr->str};

                if(token_str == u8"-add")
                {
                    if(expecting_pattern) [[unlikely]]
                    {
                        switch(scan_mode)
                        {
                            case mode_type::none:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::add_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::rm_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::symlink_escape_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            [[unlikely]] default:
                            {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                                ::std::unreachable();
                            }
                        }

                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }

                    scan_mode = mode_type::add_mode;
                    expecting_pattern = true;
                    ++scan_ptr;
                    continue;
                }

                if(token_str == u8"-rm")
                {
                    if(expecting_pattern) [[unlikely]]
                    {
                        switch(scan_mode)
                        {
                            case mode_type::none:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::add_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::rm_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::symlink_escape_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            [[unlikely]] default:
                            {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                                ::std::unreachable();
                            }
                        }
                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }

                    scan_mode = mode_type::rm_mode;
                    expecting_pattern = true;
                    ++scan_ptr;
                    continue;
                }

                if(token_str == u8"--symlink-escape-nonwasi")
                {
                    if(expecting_pattern) [[unlikely]]
                    {
                        switch(scan_mode)
                        {
                            case mode_type::none:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" or ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::add_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-add\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::rm_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"-rm\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            case mode_type::symlink_escape_mode:
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Empty pattern list after ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"--symlink-escape-nonwasi\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                break;
                            }
                            [[unlikely]] default:
                            {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                                ::std::unreachable();
                            }
                        }

                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }

                    scan_mode = mode_type::symlink_escape_mode;
                    expecting_pattern = true;
                    ++scan_ptr;
                    continue;
                }

                if(scan_mode == mode_type::add_mode)
                {
                    ++add_count;
                    expecting_pattern = false;
                }
                else if(scan_mode == mode_type::rm_mode)
                {
                    ++rm_count;
                    expecting_pattern = false;
                }
                else if(scan_mode == mode_type::symlink_escape_mode)
                {
                    ++symlink_escape_count;
                    expecting_pattern = false;
                }
                else [[unlikely]]
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                        u8"[error] ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Pattern \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        token_str,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" must follow ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        u8"-add",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8" or ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        u8"-rm",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8" or ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        u8"--symlink-escape-nonwasi\n\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                    return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                }

                ++scan_ptr;
            }

            // Check if we ended with expecting_pattern (empty list)
            if(expecting_pattern) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Empty pattern list after ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    u8"-add",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8" or ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    u8"-rm",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8" or ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    u8"--symlink-escape-nonwasi\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }
        }

        if(add_count) { entry.add_patterns.reserve(add_count); }
        if(rm_count) { entry.rm_patterns.reserve(rm_count); }
        if(symlink_escape_count) { entry.symlink_escape_patterns.reserve(symlink_escape_count); }
        if(add_count) { entry.add_automata.reserve(add_count); }
        if(rm_count) { entry.rm_automata.reserve(rm_count); }
        if(symlink_escape_count) { entry.symlink_escape_automata.reserve(symlink_escape_count); }

        while(param_cursor != para_end && param_cursor->type == ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg)
        {
            auto const arg_str{param_cursor->str};

            if(arg_str == u8"-add")
            {
                current_mode = mode_type::add_mode;
                param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++param_cursor;
                continue;
            }
            else if(arg_str == u8"-rm")
            {
                current_mode = mode_type::rm_mode;
                param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++param_cursor;
                continue;
            }
            else if(arg_str == u8"--symlink-escape-nonwasi")
            {
                current_mode = mode_type::symlink_escape_mode;
                param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++param_cursor;
                continue;
            }

            // Compile pattern tokens and build NFA (validate at the same time)
            auto const parse_result{::uwvm2::imported::wasi::wasip1::mount_root::parse_pattern(arg_str)};
            auto const& err{parse_result.error};
            auto const& tokens{parse_result.tokens};

            if(err.has_error) [[unlikely]]
            {
                // Add a lock to enable single-pass output.

                // No copies will be made here.
                auto u8log_output_osr{::fast_io::operations::output_stream_ref(::uwvm2::uwvm::io::u8log_output)};
                // Add raii locks while unlocking operations
                ::fast_io::operations::decay::stream_ref_decay_lock_guard u8log_output_lg{
                    ::fast_io::operations::decay::output_stream_mutex_ref_decay(u8log_output_osr)};
                // No copies will be made here.
                auto u8log_output_ul{::fast_io::operations::decay::output_stream_unlocked_ref_decay(u8log_output_osr)};

                ::fast_io::io::perr(u8log_output_ul,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    u8"--wasi-mount-root ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN));

                if(current_mode == mode_type::add_mode) { ::fast_io::io::perr(u8log_output_ul, u8"-add "); }
                else if(current_mode == mode_type::rm_mode) { ::fast_io::io::perr(u8log_output_ul, u8"-rm "); }

                // Print pattern with error position highlighted (render as text instead of numeric code points)
                {
                    ::std::size_t const pattern_len{arg_str.size()};
                    ::std::size_t err_pos{err.error_pos};
                    if(err_pos >= pattern_len) { err_pos = pattern_len ? (pattern_len - 1uz) : 0uz; }

                    // Build non-owning views to avoid copies
                    ::uwvm2::utils::container::u8string_view left_view{};
                    char8_t mid{};  // single error character
                    ::uwvm2::utils::container::u8string_view right_view{};

                    if(pattern_len != 0uz)
                    {
                        mid = arg_str.index_unchecked(err_pos);
                        if(err_pos != 0uz) { left_view = arg_str.subview(0uz, err_pos); }
                        if(err_pos + 1uz < pattern_len) { right_view = arg_str.subview(err_pos + 1uz, pattern_len - (err_pos + 1uz)); }

                        ::fast_io::io::perr(u8log_output_ul,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            left_view,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                            ::fast_io::mnp::chvw(mid),
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            right_view);
                    }
                    else
                    {
                        // No content; just print nothing around
                        ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE), left_view);
                    }
                }

                ::fast_io::io::perr(u8log_output_ul,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    u8" (",
                                    err.error_message,
                                    u8")\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                // u8log_output_lg unlocked here
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }

            // Add pattern to appropriate list and compiled automata
            if(current_mode == mode_type::add_mode)
            {
                entry.add_patterns.emplace_back(arg_str);
                entry.add_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::rm_mode)
            {
                entry.rm_patterns.emplace_back(arg_str);
                entry.rm_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::symlink_escape_mode)
            {
                entry.symlink_escape_patterns.emplace_back(arg_str);
                entry.symlink_escape_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            // Note: Pattern validation already done in pre-scan, so this should not happen

            param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
            ++param_cursor;
        }

        ::uwvm2::uwvm::wasm::storage::wasip1_env.mount_root = ::std::move(entry);

        return ::uwvm2::utils::cmdline::parameter_return_type::def;
    }
}  // namespace uwvm2::uwvm::cmdline::params::details

// This cpp may not be the end of the translation unit, it may be included in other cpp files. So it needs to be pop.
// macro
#include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <uwvm2/utils/macro/pop_macros.h>

