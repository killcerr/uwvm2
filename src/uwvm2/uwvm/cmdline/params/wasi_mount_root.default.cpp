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
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/cmdline/params/impl.h>
#endif

namespace uwvm2::uwvm::cmdline::params::details
{

    // Pattern matching error result
    struct pattern_match_error
    {
        bool has_error{};
        ::std::size_t error_pos{};
        ::uwvm2::utils::container::u8string_view error_message{};
    };

    // Pattern token types
    enum class pattern_token_type : unsigned
    {
        literal,      // normal character
        star,         // *
        double_star,  // **
        question,     // ?
        alternatives  // {a,b,c}
    };

    struct pattern_token
    {
        pattern_token_type type{};
        ::uwvm2::utils::container::u8string data{};                                             // for literal and alternatives
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string> alternatives{};  // for alternatives
    };

    struct pattern_token_error_and_vector_pattern_tokens
    {
        pattern_match_error error{};
        ::uwvm2::utils::container::vector<pattern_token> tokens{};
    };

    // Parse pattern into tokens
    inline pattern_token_error_and_vector_pattern_tokens parse_pattern(::uwvm2::utils::container::u8string_view pattern) noexcept
    {
        ::uwvm2::utils::container::vector<pattern_token> tokens{};

        ::std::size_t const pattern_size{pattern.size()};
        // Upper bound: token count cannot exceed pattern length
        tokens.reserve(pattern_size);

        ::std::size_t i{};
        ::uwvm2::utils::container::u8string current_literal{};
        // Upper bound: a literal cannot exceed pattern length
        current_literal.reserve(pattern_size);

        auto flush_literal{[&]() noexcept
                           {
                               if(!current_literal.empty())
                               {
                                   tokens.emplace_back_unchecked(pattern_token{.type = pattern_token_type::literal, .data = ::std::move(current_literal)});
                                   current_literal = {};
                                   current_literal.reserve(pattern_size);
                               }
                           }};

        while(i < pattern.size())
        {
            char8_t const ch{pattern.index_unchecked(i)};

            // Handle escape sequences
            if(ch == u8'\\' && i + 1uz < pattern.size())
            {
                char8_t const next{pattern.index_unchecked(i + 1uz)};
                current_literal.push_back_unchecked(next);
                i += 2uz;
                continue;
            }

            // Handle **
            if(ch == u8'*' && i + 1uz < pattern.size() && pattern.index_unchecked(i + 1uz) == u8'*')
            {
                // Check if ** is properly surrounded by / or at boundaries
                bool valid_double_star{};
                if(i == 0uz || pattern.index_unchecked(i - 1uz) == u8'/')
                {
                    if(i + 2uz >= pattern.size() || pattern.index_unchecked(i + 2uz) == u8'/') { valid_double_star = true; }
                }

                if(valid_double_star)
                {
                    flush_literal();
                    tokens.emplace_back_unchecked(pattern_token{.type = pattern_token_type::double_star});
                    i += 2uz;
                    continue;
                }
                else [[unlikely]]
                {
                    return {
                        pattern_match_error{.has_error = true, .error_pos = i, .error_message = u8"** must be surrounded by / or at path boundaries"},
                        ::std::move(tokens)
                    };
                }
            }

            // Handle *
            if(ch == u8'*')
            {
                flush_literal();
                tokens.emplace_back_unchecked(pattern_token{.type = pattern_token_type::star});
                ++i;
                continue;
            }

            // Handle ?
            if(ch == u8'?')
            {
                flush_literal();
                tokens.emplace_back_unchecked(pattern_token{.type = pattern_token_type::question});
                ++i;
                continue;
            }

            // Handle {alternatives}
            if(ch == u8'{')
            {
                flush_literal();

                ::std::size_t const brace_start{i};
                ::std::size_t brace_depth{1uz};
                ::std::size_t j{i + 1uz};

                // First pass: find matching '}', count top-level commas
                ::std::size_t top_level_commas{};
                while(j < pattern.size() && brace_depth != 0uz)
                {
                    char8_t const ch2{pattern.index_unchecked(j)};

                    if(ch2 == u8'\\' && j + 1uz < pattern.size())
                    {
                        j += 2uz;
                        continue;
                    }

                    if(ch2 == u8'{')
                    {
                        ++brace_depth;
                        ++j;
                        continue;
                    }
                    else if(ch2 == u8'}')
                    {
                        --brace_depth;
                        if(brace_depth == 0uz) { break; }
                        ++j;
                        continue;
                    }
                    else if(ch2 == u8',' && brace_depth == 1uz)
                    {
                        ++top_level_commas;
                        ++j;
                        continue;
                    }

                    ++j;
                }

                if(brace_depth != 0uz) [[unlikely]]
                {
                    return {
                        pattern_match_error{.has_error = true, .error_pos = brace_start, .error_message = u8"Unmatched '{', missing closing '}'"},
                        ::std::move(tokens)
                    };
                }

                ::std::size_t const content_begin{i + 1uz};
                ::std::size_t const content_end{j};  // index of '}'
                ::std::size_t const alt_count{top_level_commas + 1uz};

                if(content_end == content_begin) [[unlikely]]
                {
                    return {
                        pattern_match_error{.has_error = true, .error_pos = brace_start, .error_message = u8"Empty alternatives in {}"},
                        ::std::move(tokens)
                    };
                }

                ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string> alternatives{};
                alternatives.reserve(alt_count);

                ::uwvm2::utils::container::u8string current_alt{};
                // Upper bound for a single alternative length: content span length
                current_alt.reserve(content_end - content_begin);

                // Second pass: build alternatives with capacity guarantees
                ::std::size_t k{content_begin};
                ::std::size_t inner_depth{1uz};
                while(k < content_end)
                {
                    char8_t const ch2{pattern.index_unchecked(k)};

                    if(ch2 == u8'\\' && k + 1uz < content_end)
                    {
                        current_alt.push_back_unchecked(pattern.index_unchecked(k + 1uz));
                        k += 2uz;
                        continue;
                    }

                    if(ch2 == u8'{')
                    {
                        ++inner_depth;
                        current_alt.push_back_unchecked(ch2);
                        ++k;
                        continue;
                    }
                    else if(ch2 == u8'}')
                    {
                        --inner_depth;
                        if(inner_depth >= 1uz) { current_alt.push_back_unchecked(ch2); }
                        ++k;
                        continue;
                    }
                    else if(ch2 == u8',' && inner_depth == 1uz)
                    {
                        alternatives.emplace_back_unchecked(::std::move(current_alt));
                        current_alt = {};
                        current_alt.reserve(content_end - content_begin);
                        ++k;
                        continue;
                    }

                    current_alt.push_back_unchecked(ch2);
                    ++k;
                }

                alternatives.emplace_back_unchecked(::std::move(current_alt));

                pattern_token tok{.type = pattern_token_type::alternatives, .alternatives = ::std::move(alternatives)};
                tokens.emplace_back_unchecked(::std::move(tok));
                i = content_end + 1uz;
                continue;
            }

            // Regular character
            current_literal.push_back_unchecked(ch);
            ++i;
        }

        flush_literal();
        return {.tokens = ::std::move(tokens)};
    }

    // Match a path against parsed pattern tokens
    inline bool match_pattern_tokens(::uwvm2::utils::container::u8string_view path,
                                     ::uwvm2::utils::container::vector<pattern_token> const& tokens,
                                     ::std::size_t token_idx,
                                     ::std::size_t path_idx) noexcept
    {
        if(token_idx >= tokens.size()) { return path_idx >= path.size(); }

        auto const& token{tokens.index_unchecked(token_idx)};

        switch(token.type)
        {
            case pattern_token_type::literal:
            {
                auto const& lit{token.data};
                if(path_idx + lit.size() > path.size()) { return false; }

                for(::std::size_t i{}; i < lit.size(); ++i)
                {
                    if(path.index_unchecked(path_idx + i) != lit.index_unchecked(i)) { return false; }
                }

                return match_pattern_tokens(path, tokens, token_idx + 1, path_idx + lit.size());
            }
            case pattern_token_type::star:
            {
                // * matches any characters except /
                for(::std::size_t i{path_idx}; i <= path.size(); ++i)
                {
                    // Check if we've hit a /
                    if(i < path.size() && path.index_unchecked(i) == u8'/') { break; }

                    if(match_pattern_tokens(path, tokens, token_idx + 1, i)) { return true; }
                }

                return false;
            }
            case pattern_token_type::double_star:
            {
                // ** matches any characters including /
                for(::std::size_t i{path_idx}; i <= path.size(); ++i)
                {
                    if(match_pattern_tokens(path, tokens, token_idx + 1, i)) { return true; }
                }

                return false;
            }
            case pattern_token_type::question:
            {
                // ? matches any single character except /
                if(path_idx >= path.size() || path.index_unchecked(path_idx) == u8'/') { return false; }

                return match_pattern_tokens(path, tokens, token_idx + 1, path_idx + 1);
            }
            case pattern_token_type::alternatives:
            {
                // Try each alternative
                for(auto const& alt: token.alternatives)
                {
                    if(path_idx + alt.size() > path.size()) { continue; }

                    bool match{true};
                    for(::std::size_t i{}; i < alt.size(); ++i)
                    {
                        if(path.index_unchecked(path_idx + i) != alt.index_unchecked(i))
                        {
                            match = false;
                            break;
                        }
                    }

                    if(match && match_pattern_tokens(path, tokens, token_idx + 1, path_idx + alt.size())) { return true; }
                }

                return false;
            }
            [[unlikely]] default:
            {
                return false;
            }
        }

        return false;
    }

    // Mount root entry
    struct mount_root_entry
    {
        ::uwvm2::utils::container::u8string_view root_dir{};
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> add_patterns{};
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> rm_patterns{};
    };

    // Global storage for mount roots (temporary, should be integrated with WASI implementation)
    inline ::uwvm2::utils::container::vector<mount_root_entry> mount_roots{};

    UWVM_GNU_COLD extern void wasi_mount_root_pretreatment(char8_t const* const*& argv_curr,
                                                           char8_t const* const* argv_end,
                                                           ::uwvm2::utils::container::vector<::uwvm2::utils::cmdline::parameter_parsing_results>& pr) noexcept
    {
        // argv_curr points to --wasi-mount-root or -Iroot
        // We need to process: dir -add ... -rm ... until we hit another parameter

        // Pre-scan to reserve exact additional capacity for pr before using *_unchecked
        {
            ::std::size_t entries_to_add{};  // not include parameter itself
            auto scanp{argv_curr + 1u};
            if(scanp != argv_end)
            {
                ::uwvm2::utils::container::u8cstring_view v{::fast_io::mnp::os_c_str(*scanp)};
                if(!v.empty() && v.front_unchecked() != u8'-')
                {
                    ++entries_to_add;  // root dir
                    ++scanp;

                    bool stop_scanning{};
                    while(scanp != argv_end && !stop_scanning)
                    {
                        ::uwvm2::utils::container::u8cstring_view v2{::fast_io::mnp::os_c_str(*scanp)};

                        if(v2.empty()) [[unlikely]]
                        {
                            ++scanp;
                            continue;
                        }

                        if(v2 == u8"-add" || v2 == u8"-rm")
                        {
                            ++entries_to_add;  // the option itself
                            ++scanp;

                            while(scanp != argv_end)
                            {
                                ::uwvm2::utils::container::u8cstring_view v3{::fast_io::mnp::os_c_str(*scanp)};

                                if(v3.empty()) [[unlikely]]
                                {
                                    ++scanp;
                                    continue;
                                }

                                if(v3.front_unchecked() == u8'-')
                                {
                                    if(v3 == u8"-add" || v3 == u8"-rm") { break; }
                                    else
                                    {
                                        stop_scanning = true;
                                        break;
                                    }
                                }

                                ++entries_to_add;  // a pattern
                                ++scanp;
                            }

                            continue;
                        }
                        else if(v2.front_unchecked() == u8'-')
                        {
                            break;  // another parameter reached
                        }
                        else
                        {
                            ++entries_to_add;  // unexpected non-option argument after patterns
                            ++scanp;
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

        // Process -add and -rm options
        while(argv_curr != argv_end)
        {
            curr_view = ::uwvm2::utils::container::u8cstring_view{::fast_io::mnp::os_c_str(*argv_curr)};

            if(curr_view.empty()) [[unlikely]]
            {
                ++argv_curr;
                continue;
            }

            // Check if this is -add or -rm
            if(curr_view == u8"-add" || curr_view == u8"-rm")
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

                    // Stop if we hit -add, -rm, or another parameter
                    if(curr_view.front_unchecked() == u8'-')
                    {
                        if(curr_view == u8"-add" || curr_view == u8"-rm") { break; }
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
        auto currp1{para_curr + 1};

        // Check for root directory argument
        if(currp1 == para_end || currp1->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
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

        mount_root_entry entry{};
        entry.root_dir = ::fast_io::u8string_view{currp1->str};
        currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
        ++currp1;

        // Process -add and -rm options
        enum class mode_type : unsigned
        {
            none,
            add_mode,
            rm_mode
        };

        mode_type current_mode{mode_type::none};

        // Pre-scan to reserve capacities for add/rm patterns and validate structure
        ::std::size_t add_count{};
        ::std::size_t rm_count{};

        {
            auto scanp{currp1};
            mode_type scan_mode{mode_type::none};
            bool expecting_pattern{false};

            while(scanp != para_end && scanp->type == ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg)
            {
                auto const s{scanp->str};
                if(s == u8"-add")
                {
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
                                            u8"-rm\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }

                    scan_mode = mode_type::add_mode;
                    expecting_pattern = true;
                    ++scanp;
                    continue;
                }
                if(s == u8"-rm")
                {
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
                                            u8"-rm\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                        return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                    }
                    scan_mode = mode_type::rm_mode;
                    expecting_pattern = true;
                    ++scanp;
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
                else [[unlikely]]
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                        u8"[error] ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Pattern \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        s,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" must follow -add or -rm\n\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                    return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
                }
                ++scanp;
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
                                    u8"-rm\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
            }
        }
        
        if(add_count) { entry.add_patterns.reserve(add_count); }
        if(rm_count) { entry.rm_patterns.reserve(rm_count); }

        while(currp1 != para_end && currp1->type == ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg)
        {
            auto const arg_str{currp1->str};

            if(arg_str == u8"-add")
            {
                current_mode = mode_type::add_mode;
                currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++currp1;
                continue;
            }
            else if(arg_str == u8"-rm")
            {
                current_mode = mode_type::rm_mode;
                currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++currp1;
                continue;
            }

            // Validate pattern
            auto const [err, tokens]{parse_pattern(arg_str)};

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

            // Add pattern to appropriate list
            if(current_mode == mode_type::add_mode) { entry.add_patterns.emplace_back_unchecked(arg_str); }
            else if(current_mode == mode_type::rm_mode) { entry.rm_patterns.emplace_back_unchecked(arg_str); }
            // Note: Pattern validation already done in pre-scan, so this should not happen

            currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
            ++currp1;
        }

        mount_roots.emplace_back(::std::move(entry));

        return ::uwvm2::utils::cmdline::parameter_return_type::def;
    }
}  // namespace uwvm2::uwvm::cmdline::params::details

// This cpp may not be the end of the translation unit, it may be included in other cpp files. So it needs to be pop.
// macro
#include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <uwvm2/utils/macro/pop_macros.h>

