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
# include <uwvm2/utils/utf/impl.h>
# include <uwvm2/imported/wasi/wasip1/mount_root/impl.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/cmdline/params/impl.h>
# include <uwvm2/uwvm/wasm/storage/impl.h>
#endif

namespace uwvm2::uwvm::cmdline::params::details
{
    UWVM_GNU_COLD extern void wasi_mount_dir_pretreatment(char8_t const* const*& argv_curr,
                                                          char8_t const* const* argv_end,
                                                          ::uwvm2::utils::container::vector<::uwvm2::utils::cmdline::parameter_parsing_results>& pr) noexcept
    {
        // argv_curr points to --wasi-mount-dir or -Idir
        // New syntax: <wasidir> <system_dir> [-add ...] [-rm ...] [--symlink-escape-nonwasi ...]

        // Pre-scan to reserve exact additional capacity for pr before using *_unchecked
        {
            ::std::size_t entries_to_add{};  // not include parameter itself
            auto scan_ptr{argv_curr + 1u};
            if(scan_ptr != argv_end)
            {
                ::uwvm2::utils::container::u8cstring_view wasidir_view{::fast_io::mnp::os_c_str(*scan_ptr)};
                if(!wasidir_view.empty() && wasidir_view.front_unchecked() != u8'-')
                {
                    ++entries_to_add;  // wasidir
                    ++scan_ptr;

                    if(scan_ptr != argv_end)
                    {
                        ::uwvm2::utils::container::u8cstring_view sysdir_view{::fast_io::mnp::os_c_str(*scan_ptr)};
                        if(!sysdir_view.empty() && sysdir_view.front_unchecked() != u8'-')
                        {
                            ++entries_to_add;  // system_dir
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
                                    // Unexpected non-option argument after patterns: stop scanning further for this parameter
                                    stop_scanning = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if(entries_to_add) { pr.reserve(pr.size() + entries_to_add); }
        }

        // You should not add its own parameter into pr, because it has already been handled in the command-line parser!

        ++argv_curr;
        if(argv_curr == argv_end) { return; }

        // First argument must be the WASI dir (not starting with -)
        ::uwvm2::utils::container::u8cstring_view curr_view{::fast_io::mnp::os_c_str(*argv_curr)};
        if(curr_view.empty() || curr_view.front_unchecked() == u8'-') [[unlikely]] { return; }
        pr.emplace_back_unchecked(curr_view, nullptr, ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg);
        ++argv_curr;

        // Second argument must be the system dir (not starting with -)
        if(argv_curr == argv_end) { return; }
        curr_view = ::uwvm2::utils::container::u8cstring_view{::fast_io::mnp::os_c_str(*argv_curr)};
        if(curr_view.empty() || curr_view.front_unchecked() == u8'-') [[unlikely]] { return; }
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
                // Unexpected non-option argument after patterns: stop processing this parameter here
                return;
            }
        }
    }

    UWVM_GNU_COLD extern ::uwvm2::utils::cmdline::parameter_return_type
        wasi_mount_dir_callback([[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_begin,
                                ::uwvm2::utils::cmdline::parameter_parsing_results* para_curr,
                                ::uwvm2::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        auto param_cursor{para_curr + 1};

        // Check for wasi mount dir
        if(param_cursor == para_end || param_cursor->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Usage: ",
                                ::uwvm2::utils::cmdline::print_usage(::uwvm2::uwvm::cmdline::params::wasi_mount_dir),
                                u8"\n\n");

            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        // Parse wasidir
        ::uwvm2::utils::container::u8string_view wasidir{param_cursor->str};
        param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
        ++param_cursor;

        // Check system dir argument
        if(param_cursor == para_end || param_cursor->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Missing ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                u8"<system dir>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8" after ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                u8"<wasi dir>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8" for ",
                                ::uwvm2::utils::cmdline::print_usage(::uwvm2::uwvm::cmdline::params::wasi_mount_dir),
                                u8"\n\n");
            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        ::uwvm2::imported::wasi::wasip1::mount_root::mount_root_entry entry{};
        entry.root_dir = ::fast_io::u8string_view{param_cursor->str};
        param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
        ++param_cursor;

        // Validate wasidir (absolute and relative modes):
        // Absolute mode: POSIX-like absolute path, forbid illegal chars (\\, *, ?, ", <, >, |, :),
        //                forbid '//' and any path segment equal to '.' or '..' (but allow '.' as the whole path),
        //                allow other dots like '...'/'.config'. Additionally, require valid UTF-8 and forbid NUL inside view.
        // Relative mode: allow '.' or './name/...'; after the leading '.', segments cannot be '.' or '..'.

        if(wasidir.empty()) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                u8"<wasi dir>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8": cannot be empty\n\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        // Detect relative mode: any path not starting with '/' is treated as relative (including '.')
        bool const wasidir_is_relative{wasidir.front_unchecked() != u8'/'};

        if(wasidir_is_relative)
        {
            // check not "."
            if(wasidir != u8".")
            {
                bool prev_slash{};  // we don't start with '/'
                ::std::size_t seg_len{};
                bool seg_only_dots{true};

                for(::std::size_t i{}; i != wasidir.size(); ++i)
                {
                    auto const ch{wasidir.index_unchecked(i)};
                    if(ch == u8'/')
                    {
                        if(prev_slash) [[unlikely]]
                        {
                            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                u8"uwvm: ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                u8"[error] ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8"Invalid ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                u8"<wasi dir>",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8": must not contain '//', in ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                                wasidir,
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8" at offset ",
                                                i,
                                                u8"\n\n",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;  // forbid '//'
                        }

                        if(seg_len != 0uz)
                        {
                            // forbid '.' or '..' segment
                            if(seg_only_dots && (seg_len == 1uz || seg_len == 2uz)) [[unlikely]]
                            {
                                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                    u8"uwvm: ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                    u8"[error] ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8"Invalid ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                    u8"<wasi dir>",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8": path segments './', '/.' or '..' are not allowed, in ",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                                    wasidir,
                                                    u8"\n\n",
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                            }
                        }

                        seg_len = 0uz;
                        seg_only_dots = true;
                        prev_slash = true;
                    }
                    else
                    {
                        // POSIX/WASI: do not restrict characters beyond '/' and dot-segment rules

                        prev_slash = false;
                        ++seg_len;
                        if(seg_only_dots && ch != u8'.') { seg_only_dots = false; }
                    }
                }

                if(seg_len != 0uz)
                {
                    if(seg_only_dots && (seg_len == 1uz || seg_len == 2uz)) [[unlikely]]
                    {
                        ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                            u8"uwvm: ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                            u8"[error] ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"Invalid ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                            u8"<wasi dir>",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8": path segments './', '/.' or '..' are not allowed, in ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                            wasidir,
                                            u8"\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }
                }
            }
        }
        else
        {
            bool prev_slash{};
            ::std::size_t seg_len{};
            bool seg_only_dots{true};

            for(auto const ch: wasidir)
            {
                if(ch == u8'/')
                {
                    if(prev_slash) [[unlikely]]
                    {
                        ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                            u8"uwvm: ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                            u8"[error] ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"Invalid ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                            u8"<wasi dir>",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8": must not contain '//', in ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                            wasidir,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }

                    if(seg_len != 0uz)
                    {
                        if(seg_only_dots && (seg_len == 1uz || seg_len == 2uz)) [[unlikely]]
                        {
                            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                u8"uwvm: ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                u8"[error] ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8"Invalid ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                u8"<wasi dir>",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8": path segments '.' or '..' are not allowed in absolute paths, in ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                                wasidir,
                                                u8"\n\n",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                        }
                    }

                    seg_len = 0uz;
                    seg_only_dots = true;
                    prev_slash = true;
                }
                else
                {
                    // POSIX/WASI: do not restrict characters beyond '/' and dot-segment rules

                    prev_slash = false;
                    ++seg_len;
                    if(seg_only_dots && ch != u8'.') { seg_only_dots = false; }
                }
            }

            if(seg_len != 0uz)
            {
                if(seg_only_dots && (seg_len == 1uz || seg_len == 2uz)) [[unlikely]]
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                        u8"[error] ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Invalid ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        u8"<wasi dir>",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8": path segments '.' or '..' are not allowed in absolute paths, in ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        wasidir,
                                        u8"\n\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                    return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                }
            }
        }

        // check utf8
        auto const u8res{
            ::uwvm2::utils::utf::check_legal_utf8<::uwvm2::utils::utf::utf8_specification::utf8_rfc3629_and_zero_illegal>(wasidir.cbegin(), wasidir.cend())};
        if(u8res.err != ::uwvm2::utils::utf::utf_error_code::success) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                u8"<wasi dir>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8": invalid UTF-8 (",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                ::uwvm2::utils::utf::get_utf_error_description<char8_t>(u8res.err),
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8")\n\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        // Canonicalize wasidir: remove trailing '/' (except root "/") for stable comparisons
        constexpr auto canonicalize_wasidir{[](::uwvm2::utils::container::u8string_view p) constexpr noexcept -> ::uwvm2::utils::container::u8string_view
                                            {
                                                if(p == u8".") { return p; }
                                                auto n{p.size()};
                                                while(n > 1uz && p.index_unchecked(n - 1uz) == u8'/') { --n; }
                                                return ::uwvm2::utils::container::u8string_view{p.data(), n};
                                            }};

        // Canonicalize relative wasidir: forbid './' prefix; keep '.' as is; trim trailing '/'.
        constexpr auto canonicalize_wasidir_rel{[](::uwvm2::utils::container::u8string_view p) constexpr noexcept -> ::uwvm2::utils::container::u8string_view
                                                {
                                                    if(p == u8".") { return p; }
                                                    // p is a pure relative path like 'b/xxx'; remove trailing '/'
                                                    auto n{p.size()};
                                                    while(n != 0uz && p.index_unchecked(n - 1uz) == u8'/') { --n; }
                                                    if(n == 0uz) { return ::uwvm2::utils::container::u8string_view{u8".", 1uz}; }
                                                    return ::uwvm2::utils::container::u8string_view{p.data(), n};
                                                }};

        auto const wasidir_norm{wasidir_is_relative ? canonicalize_wasidir_rel(wasidir) : canonicalize_wasidir(wasidir)};

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
            bool expecting_pattern{};

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
                                    u8"--wasi-mount-dir ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN));

                if(current_mode == mode_type::add_mode) { ::fast_io::io::perr(u8log_output_ul, u8"-add "); }
                else if(current_mode == mode_type::rm_mode) { ::fast_io::io::perr(u8log_output_ul, u8"-rm "); }
                else if(current_mode == mode_type::symlink_escape_mode) { ::fast_io::io::perr(u8log_output_ul, u8"--symlink-escape-nonwasi "); }

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
            // Normalize trailing slashes for storage (keep "/" as-is; otherwise strip all trailing '/').
            constexpr auto normalize_trailing_slashes_view{[](::uwvm2::utils::container::u8string_view v) constexpr noexcept
                                                           {
                                                               if(v.size() <= 1uz) { return v; }
                                                               ::std::size_t end{v.size()};
                                                               while(end > 1uz && v.index_unchecked(end - 1uz) == u8'/') { --end; }
                                                               return v.subview(0uz, end);
                                                           }};
            if(current_mode == mode_type::add_mode)
            {
                entry.add_patterns.emplace_back(normalize_trailing_slashes_view(arg_str));
                entry.add_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::rm_mode)
            {
                entry.rm_patterns.emplace_back(normalize_trailing_slashes_view(arg_str));
                entry.rm_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::symlink_escape_mode)
            {
                entry.symlink_escape_patterns.emplace_back(normalize_trailing_slashes_view(arg_str));
                entry.symlink_escape_automata.emplace_back(::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(tokens));
            }
            // Note: Pattern validation already done in pre-scan, so this should not happen

            param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
            ++param_cursor;
        }

        // Conflict check with existing mounts: disallow prefix conflicts both directions
        auto& env{::uwvm2::uwvm::wasm::storage::default_wasi_env};
        for(auto const& mr: env.mount_dir_roots)
        {
            auto const existing{mr.preload_dir};
            // Supports normalization of both absolute paths and relative paths (including “.”).
            auto const existing_norm{(!existing.empty() && existing.front_unchecked() == u8'/') ? canonicalize_wasidir(existing)
                                                                                                : canonicalize_wasidir_rel(existing)};
            // normalize: we assume stored wasidir was validated and absolute
            constexpr auto starts_with{[](::uwvm2::utils::container::u8string_view a, ::uwvm2::utils::container::u8string_view b) constexpr noexcept -> bool
                                       {
                                           // Special case: '/' is the prefix of any absolute path
                                           if(b == u8"/") { return (!a.empty() && a.front_unchecked() == u8'/'); }

                                           // Special case: for relative paths, '.' is the prefix of any relative path
                                           if(!a.empty() && a.front_unchecked() != u8'/' && b == u8".") { return true; }

                                           if(a.size() < b.size()) { return false; }
                                           if(a.size() == b.size()) { return a == b; }

#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                           if(a.size() <= b.size()) [[unlikely]] { ::uwvm2::utils::debug::trap_and_inform_bug_pos(); }
#endif

                                           return (::uwvm2::utils::container::u8string_view{a.data(), b.size()} == b) && (a.index_unchecked(b.size()) == u8'/');
                                       }};

            if(wasidir_norm == existing_norm) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate mount ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    u8"<wasi dir>",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8": ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    wasidir_norm,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8" already mounted.\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }

            if(starts_with(wasidir_norm, existing_norm) || starts_with(existing_norm, wasidir_norm)) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Mount conflict: disallow overlapping prefixes between ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    wasidir_norm,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8" and ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                    existing_norm,
                                    u8"\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }
        }

        // Record into default_wasi_env
        ::uwvm2::imported::wasi::wasip1::environment::mount_dir_root_t mdr{};
        mdr.preload_dir = wasidir_norm;
        mdr.entry = ::std::move(entry);
        env.mount_dir_roots.emplace_back(::std::move(mdr));

        return ::uwvm2::utils::cmdline::parameter_return_type::def;
    }
}  // namespace uwvm2::uwvm::cmdline::params::details

// This cpp may not be the end of the translation unit, it may be included in other cpp files. So it needs to be pop.
// macro
#include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <uwvm2/utils/macro/pop_macros.h>

