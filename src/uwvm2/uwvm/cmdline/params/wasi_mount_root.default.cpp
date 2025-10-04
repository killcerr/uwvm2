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
    /// @brief Error information produced during wildcard pattern tokenization.
    struct pattern_match_error
    {
        bool has_error{};
        ::std::size_t error_pos{};
        ::uwvm2::utils::container::u8string_view error_message{};
    };

    // Pattern token types
    /// @brief Token kinds for glob patterns supporting literal, *, **, ?, and {a,b} alternatives.
    enum class pattern_token_type : unsigned
    {
        literal,      // normal character
        star,         // *
        double_star,  // **
        question,     // ?
        alternatives  // {a,b,c}
    };

    /// @brief One parsed pattern token; literal text or a wildcard operator.
    struct pattern_token
    {
        pattern_token_type type{};
        ::uwvm2::utils::container::u8string data{};                                             // for literal and alternatives
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string> alternatives{};  // for alternatives
    };

    /// @brief Pair of tokenization error and the resulting token list.
    struct pattern_token_error_and_vector_pattern_tokens
    {
        pattern_match_error error{};
        ::uwvm2::utils::container::vector<pattern_token> tokens{};
    };

    // NFA-based compiled automaton for a single wildcard pattern
    /// @brief Edge kinds for NFA transitions when matching glob patterns.
    enum class nfa_edge_type : unsigned
    {
        char_eq,        // match exact character
        any_not_slash,  // match any single character except '/'
        any_char        // match any single character including '/'
    };

    /// @brief One consuming transition from an NFA state.
    struct nfa_edge
    {
        nfa_edge_type type{};
        char8_t ch{};  // only when type == char_eq
        ::std::size_t to{};
    };

    /// @brief One NFA state with epsilon transitions and consuming edges.
    struct nfa_state
    {
        ::uwvm2::utils::container::vector<::std::size_t> eps{};  // epsilon transitions
        ::uwvm2::utils::container::vector<nfa_edge> edges{};     // consuming transitions
        bool accepting{};
    };

    /// @brief Compiled NFA for a single glob pattern; used for repeated matching.
    struct compiled_pattern_automaton
    {
        ::uwvm2::utils::container::vector<nfa_state> nfa{};
        ::std::size_t start{};
    };

    /// @brief Create a new NFA state and return its index.
    inline constexpr ::std::size_t nfa_new_state(::uwvm2::utils::container::vector<nfa_state>& nfa) noexcept
    {
        auto const idx{nfa.size()};
        nfa.emplace_back(nfa_state{});
        return idx;
    }

    inline constexpr ::std::size_t nfa_new_state_unchecked(::uwvm2::utils::container::vector<nfa_state>& nfa) noexcept
    {
        auto const idx{nfa.size()};
        nfa.emplace_back_unchecked(nfa_state{});
        return idx;
    }

    /// @brief Add an epsilon transition in the NFA from -> to.
    /// @param nfa The NFA to add the epsilon transition to.
    /// @param from The source state. (unchecked)
    /// @param to The target state.
    inline constexpr void nfa_add_eps(::uwvm2::utils::container::vector<nfa_state>& nfa, ::std::size_t from, ::std::size_t to) noexcept
    {
        nfa.index_unchecked(from).eps.emplace_back(to);
    }

    inline constexpr void nfa_add_eps_unchecked(::uwvm2::utils::container::vector<nfa_state>& nfa, ::std::size_t from, ::std::size_t to) noexcept
    {
        nfa.index_unchecked(from).eps.emplace_back_unchecked(to);
    }

    /// @brief Add a consuming transition with given edge type and optional character.
    /// @param nfa The NFA to add the edge to.
    /// @param from The source state. (unchecked)
    /// @param to The target state.
    /// @param type The type of edge to add.
    /// @param ch The character to add (optional).
    inline constexpr void nfa_add_edge(::uwvm2::utils::container::vector<nfa_state>& nfa,
                                       ::std::size_t from,
                                       ::std::size_t to,
                                       nfa_edge_type type,
                                       char8_t match_char = u8'\0') noexcept
    {
        nfa.index_unchecked(from).edges.emplace_back(type, match_char, to);
    }

    // Build an NFA from parsed pattern tokens
    /// @brief Compile a parsed token sequence into an NFA implementing glob semantics.
    inline constexpr compiled_pattern_automaton build_nfa_from_tokens(::uwvm2::utils::container::vector<pattern_token> const& tokens) noexcept
    {
        compiled_pattern_automaton automaton{};

        auto const token_count{tokens.size()};
        if(token_count > ::std::numeric_limits<::std::size_t>::max() / 4uz - 1uz) [[unlikely]] { ::fast_io::fast_terminate(); }
        automaton.nfa.reserve(1uz + token_count * 4uz);
        automaton.start = nfa_new_state_unchecked(automaton.nfa);

        // current endpoints to connect via epsilon
        ::uwvm2::utils::container::vector<::std::size_t> endpoints{};
        endpoints.reserve(token_count + 1uz);
        endpoints.emplace_back_unchecked(automaton.start);  // No check is needed here since it is always greater than 1.

        for(auto const& token: tokens)
        {
            switch(token.type)
            {
                case pattern_token_type::literal:
                {
                    if(token.data.empty()) { break; }

                    // create a chain for literal
                    ::std::size_t const literal_entry_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, literal_entry_state); }

                    ::std::size_t last_state{literal_entry_state};
                    for(auto const literal_char: token.data)
                    {
                        auto const next_state{nfa_new_state(automaton.nfa)};
                        nfa_add_edge(automaton.nfa, last_state, next_state, nfa_edge_type::char_eq, literal_char);
                        last_state = next_state;
                    }

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(last_state);

                    break;
                }
                case pattern_token_type::question:
                {
                    ::std::size_t next_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_edge(automaton.nfa, endpoint_state, next_state, nfa_edge_type::any_not_slash); }

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(next_state);

                    break;
                }
                case pattern_token_type::star:
                {
                    // zero or more of non-slash
                    ::std::size_t repeat_non_slash_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, repeat_non_slash_state); }
                    nfa_add_edge(automaton.nfa, repeat_non_slash_state, repeat_non_slash_state, nfa_edge_type::any_not_slash);

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(repeat_non_slash_state);

                    break;
                }
                case pattern_token_type::double_star:
                {
                    // zero or more of any char
                    ::std::size_t repeat_any_char_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, repeat_any_char_state); }
                    nfa_add_edge(automaton.nfa, repeat_any_char_state, repeat_any_char_state, nfa_edge_type::any_char);

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(repeat_any_char_state);

                    break;
                }
                case pattern_token_type::alternatives:
                {
                    ::std::size_t alternatives_join_state{nfa_new_state(automaton.nfa)};
                    for(auto const& alternative: token.alternatives)
                    {
                        ::std::size_t alt_entry_state{nfa_new_state(automaton.nfa)};
                        for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, alt_entry_state); }
                        ::std::size_t last_state{alt_entry_state};
                        for(auto const alt_char: alternative)
                        {
                            auto const next_state{nfa_new_state(automaton.nfa)};
                            nfa_add_edge(automaton.nfa, last_state, next_state, nfa_edge_type::char_eq, alt_char);
                            last_state = next_state;
                        }
                        nfa_add_eps(automaton.nfa, last_state, alternatives_join_state);
                    }

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(alternatives_join_state);

                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    break;
                }
            }
        }

        // accepting tail
        ::std::size_t const accept_state{nfa_new_state(automaton.nfa)};
        automaton.nfa.index_unchecked(accept_state).accepting = true;
        for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, accept_state); }

        return automaton;
    }

    /// @brief Expand a set of NFA states by following epsilon transitions (in-place closure).
    inline constexpr void nfa_epsilon_closure(::uwvm2::utils::container::vector<nfa_state> const& nfa,
                                              ::uwvm2::utils::container::vector<::std::size_t>& state_set) noexcept
    {
        ::uwvm2::utils::container::vector<::std::size_t> dfs_stack{};
        dfs_stack.reserve(state_set.size());
        for(auto const state_index: state_set) { dfs_stack.emplace_back_unchecked(state_index); }

        ::uwvm2::utils::container::vector<bool> visited{};
        visited.resize(nfa.size());
        for(auto const state_index: state_set) { visited.index_unchecked(state_index) = true; }

        while(!dfs_stack.empty())
        {
            auto const state_index{dfs_stack.back_unchecked()};
            dfs_stack.pop_back_unchecked();
            auto const& state{nfa.index_unchecked(state_index)};
            for(auto const to: state.eps)
            {
                if(!visited.index_unchecked(to))
                {
                    visited.index_unchecked(to) = true;
                    state_set.emplace_back(to);
                    dfs_stack.emplace_back(to);
                }
            }
        }
    }

    /// @brief Check if a consuming edge can match a given character.
    inline constexpr bool nfa_edge_match(nfa_edge const& edge, char8_t input_char) noexcept
    {
        switch(edge.type)
        {
            case nfa_edge_type::char_eq: return edge.ch == input_char;
            case nfa_edge_type::any_not_slash: return input_char != u8'/';
            case nfa_edge_type::any_char:
                return true;
            [[unlikely]] default:
                return false;
        }
    }

    /// @brief Match a full path string against a compiled NFA.
    inline constexpr bool match_nfa(compiled_pattern_automaton const& automaton, ::uwvm2::utils::container::u8string_view path) noexcept
    {
        ::uwvm2::utils::container::vector<::std::size_t> current_state_set{};
        current_state_set.reserve(1uz);
        current_state_set.emplace_back_unchecked(automaton.start);  // No check is needed here since it is always greater than 1.

        nfa_epsilon_closure(automaton.nfa, current_state_set);

        for(auto const input_char: path)
        {
            ::uwvm2::utils::container::vector<::std::size_t> next_state_set{};
            next_state_set.reserve(current_state_set.size() + 4uz);

            ::uwvm2::utils::container::vector<bool> visited{};
            visited.resize(automaton.nfa.size());

            for(auto const state_index: current_state_set)
            {
                auto const& state{automaton.nfa.index_unchecked(state_index)};
                for(auto const& edge: state.edges)
                {
                    if(nfa_edge_match(edge, input_char))
                    {
                        if(!visited.index_unchecked(edge.to))
                        {
                            visited.index_unchecked(edge.to) = true;
                            next_state_set.emplace_back(edge.to);
                        }
                    }
                }
            }

            if(next_state_set.empty()) { return false; }
            nfa_epsilon_closure(automaton.nfa, next_state_set);
            current_state_set.swap(next_state_set);
        }

        for(auto const state_index: current_state_set)
        {
            if(automaton.nfa.index_unchecked(state_index).accepting) { return true; }
        }

        return false;
    }

    // Parse pattern into tokens
    /// @brief Tokenize a glob pattern; validates syntax and produces a token sequence.
    inline constexpr pattern_token_error_and_vector_pattern_tokens parse_pattern(::uwvm2::utils::container::u8string_view pattern) noexcept
    {
        ::uwvm2::utils::container::vector<pattern_token> tokens{};

        ::std::size_t const pattern_size{pattern.size()};
        // Upper bound: token count cannot exceed pattern length
        tokens.reserve(pattern_size);

        ::std::size_t pattern_index{};
        ::uwvm2::utils::container::u8string current_literal{};
        // Upper bound: a literal cannot exceed pattern length
        current_literal.reserve(pattern_size);

        auto flush_literal{[&]() noexcept
                           {
                               if(!current_literal.empty())
                               {
                                   tokens.emplace_back(pattern_token{.type = pattern_token_type::literal, .data = ::std::move(current_literal)});
                                   current_literal = {};
                                   current_literal.reserve(pattern_size);
                               }
                           }};

        while(pattern_index < pattern.size())
        {
            char8_t const current_char{pattern.index_unchecked(pattern_index)};  // safe: pattern_index < pattern.size()

            // Handle escape sequences
            if(current_char == u8'\\' && pattern_index + 1uz < pattern.size())
            {
                char8_t const escaped_char{pattern.index_unchecked(pattern_index + 1uz)};  // safe: pattern_index+1 < pattern.size()
                current_literal.push_back(escaped_char);
                pattern_index += 2uz;
                continue;
            }

            // Handle **
            if(current_char == u8'*' && pattern_index + 1uz < pattern.size() && pattern.index_unchecked(pattern_index + 1uz) == u8'*')
            {
                // Check if ** is properly surrounded by / or at boundaries
                bool valid_double_star{};
                if(pattern_index == 0uz || pattern.index_unchecked(pattern_index - 1uz) == u8'/')
                {
                    if(pattern_index + 2uz >= pattern.size() || pattern.index_unchecked(pattern_index + 2uz) == u8'/') { valid_double_star = true; }
                }

                if(valid_double_star)
                {
                    flush_literal();
                    tokens.emplace_back(pattern_token{.type = pattern_token_type::double_star});
                    pattern_index += 2uz;
                    continue;
                }
                else [[unlikely]]
                {
                    return {
                        pattern_match_error{.has_error = true,
                                            .error_pos = pattern_index,
                                            .error_message = u8"** must be surrounded by / or at path boundaries"},
                        ::std::move(tokens)
                    };
                }
            }

            // Handle *
            if(current_char == u8'*')
            {
                flush_literal();
                tokens.emplace_back(pattern_token{.type = pattern_token_type::star});
                ++pattern_index;
                continue;
            }

            // Handle ?
            if(current_char == u8'?')
            {
                flush_literal();
                tokens.emplace_back(pattern_token{.type = pattern_token_type::question});
                ++pattern_index;
                continue;
            }

            // Handle {alternatives}
            if(current_char == u8'{')
            {
                flush_literal();

                ::std::size_t const brace_start{pattern_index};
                ::std::size_t brace_depth{1uz};
                ::std::size_t cursor{pattern_index + 1uz};

                // First pass: find matching '}', count top-level commas
                ::std::size_t top_level_commas{};
                while(cursor < pattern.size() && brace_depth != 0uz)
                {
                    char8_t const next_char{pattern.index_unchecked(cursor)};  // safe: cursor < pattern.size()

                    if(next_char == u8'\\' && cursor + 1uz < pattern.size())
                    {
                        cursor += 2uz;
                        continue;
                    }

                    if(next_char == u8'{')
                    {
                        ++brace_depth;
                        ++cursor;
                        continue;
                    }
                    else if(next_char == u8'}')
                    {
                        --brace_depth;
                        if(brace_depth == 0uz) { break; }
                        ++cursor;
                        continue;
                    }
                    else if(next_char == u8',' && brace_depth == 1uz)
                    {
                        ++top_level_commas;
                        ++cursor;
                        continue;
                    }

                    ++cursor;
                }

                if(brace_depth != 0uz) [[unlikely]]
                {
                    return {
                        pattern_match_error{.has_error = true, .error_pos = brace_start, .error_message = u8"Unmatched '{', missing closing '}'"},
                        ::std::move(tokens)
                    };
                }

                ::std::size_t const content_begin{pattern_index + 1uz};
                ::std::size_t const content_end{cursor};  // index of '}'
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

                ::uwvm2::utils::container::u8string current_alternative{};
                // Upper bound for a single alternative length: content span length
                current_alternative.reserve(content_end - content_begin);

                // Second pass: build alternatives with capacity guarantees
                ::std::size_t content_cursor{content_begin};
                ::std::size_t inner_depth{1uz};
                while(content_cursor < content_end)
                {
                    char8_t const inner_char{pattern.index_unchecked(content_cursor)};  // safe: content_cursor < content_end <= pattern.size()

                    if(inner_char == u8'\\' && content_cursor + 1uz < content_end)
                    {
                        current_alternative.push_back(pattern.index_unchecked(content_cursor + 1uz));
                        content_cursor += 2uz;
                        continue;
                    }

                    if(inner_char == u8'{')
                    {
                        ++inner_depth;
                        current_alternative.push_back(inner_char);
                        ++content_cursor;
                        continue;
                    }
                    else if(inner_char == u8'}')
                    {
                        --inner_depth;
                        if(inner_depth >= 1uz) { current_alternative.push_back(inner_char); }
                        ++content_cursor;
                        continue;
                    }
                    else if(inner_char == u8',' && inner_depth == 1uz)
                    {
                        alternatives.emplace_back(::std::move(current_alternative));
                        current_alternative = {};
                        current_alternative.reserve(content_end - content_begin);
                        ++content_cursor;
                        continue;
                    }

                    current_alternative.push_back(inner_char);
                    ++content_cursor;
                }

                alternatives.emplace_back(::std::move(current_alternative));

                pattern_token alternatives_token{.type = pattern_token_type::alternatives, .alternatives = ::std::move(alternatives)};
                tokens.emplace_back(::std::move(alternatives_token));
                pattern_index = content_end + 1uz;
                continue;
            }

            // Regular character
            current_literal.push_back(current_char);
            ++pattern_index;
        }

        flush_literal();
        return {.tokens = ::std::move(tokens)};
    }

    // (Removed) legacy recursive token matching has been replaced by NFA runtime

    /// @brief One mount root configuration including raw patterns and compiled automata.
    struct mount_root_entry
    {
        ::uwvm2::utils::container::u8string_view root_dir{};  // Only existence of the root directory is validated

        // Raw patterns
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> add_patterns{};             // whitelist
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> rm_patterns{};              // blacklist
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> symlink_escape_patterns{};  // parameter patterns

        // Compiled automata for fast matching at runtime (readdir/path access)
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> add_automata{};
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> rm_automata{};
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> symlink_escape_automata{};
    };

    /// @brief Convenience wrapper to match a path over a compiled pattern automaton.
    inline constexpr bool match_compiled_pattern(::uwvm2::utils::container::u8string_view path, compiled_pattern_automaton const& automaton) noexcept
    {
        return match_nfa(automaton, path);
    }

    /// @brief Match a path against any of the compiled automata in the list.
    inline constexpr bool match_any(::uwvm2::utils::container::u8string_view path,
                                    ::uwvm2::utils::container::vector<compiled_pattern_automaton> const& automata) noexcept
    {
        for(auto const& automaton: automata)
        {
            if(match_compiled_pattern(path, automaton)) { return true; }
        }
        return false;
    }

    /// @brief Access decision for path operations
    enum class access_policy : unsigned
    {
        deny,              // not allowed
        allow,             // allowed with normal root confinement
        allow_bypass_root  // allowed and bypass root realpath checks (symlink escape)
    };

    /// @brief Evaluate access policy for a relative path under a mount root, applying precedence rules.
    inline constexpr access_policy evaluate_path_access(mount_root_entry const& entry,
                                                        ::uwvm2::utils::container::u8string_view relative_path,
                                                        bool is_symlink,
                                                        bool is_wasi_created,
                                                        bool is_symlink_creation) noexcept
    {
        // A) symlink-escape-nonwasi: highest priority
        if(is_symlink)
        {
            if(match_any(relative_path, entry.symlink_escape_automata)) { return access_policy::allow_bypass_root; }
        }
        if(is_symlink_creation)
        {
            if(match_any(relative_path, entry.symlink_escape_automata)) { return access_policy::deny; }
        }

        // B) WASI whitelist: handled by WASI, but when informed, it should override blacklist
        if(is_wasi_created) { return access_policy::allow; }

        // C) whitelist
        if(!entry.add_automata.empty() && match_any(relative_path, entry.add_automata)) { return access_policy::allow; }

        // D) blacklist
        if(!entry.rm_automata.empty() && match_any(relative_path, entry.rm_automata)) { return access_policy::deny; }

        // E) normal
        return access_policy::allow;
    }

    /// @brief Check whether a path-open request is allowed under mount rules.
    inline constexpr bool wasi_rule_allow_open(mount_root_entry const& entry,
                                               ::uwvm2::utils::container::u8string_view relative_path,
                                               bool is_symlink,
                                               bool is_wasi_created,
                                               bool is_symlink_creation,
                                               access_policy* out_policy = nullptr) noexcept
    {
        auto const pol{evaluate_path_access(entry, relative_path, is_symlink, is_wasi_created, is_symlink_creation)};
        if(out_policy) { *out_policy = pol; }
        return pol != access_policy::deny;
    }

    /// @brief Filter a single directory layer's child entries according to mount rules.
    inline constexpr void wasi_filter_directory_entries(mount_root_entry const& entry,
                                                        ::uwvm2::utils::container::u8string_view relative_dir,
                                                        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> const& child_names,
                                                        ::uwvm2::utils::container::vector<unsigned char> const& child_is_symlink,
                                                        ::uwvm2::utils::container::vector<unsigned char>& out_allowed) noexcept
    {
        out_allowed.clear();
        out_allowed.reserve(child_names.size());

        bool const dir_has_trailing_slash{!relative_dir.empty() && relative_dir.back_unchecked() == u8'/'};  // back_unchecked safe: checked empty
        for(::std::size_t child_index{}; child_index < child_names.size(); ++child_index)
        {
            auto const child_name{child_names.index_unchecked(child_index)};  // index_unchecked safe: child_index < child_names.size()

            // Build relative path: relative_dir + ['/' if needed] + name
            ::uwvm2::utils::container::u8string relative_path{};
            relative_path.reserve(relative_dir.size() + (dir_has_trailing_slash ? 0uz : 1uz) + child_name.size());
            if(!relative_dir.empty())
            {
                relative_path.append(relative_dir);
                if(!dir_has_trailing_slash) { relative_path.push_back(u8'/'); }
            }
            relative_path.append(child_name);

            auto const allow{wasi_rule_allow_open(entry,
                                                  ::uwvm2::utils::container::u8string_view{relative_path.data(), relative_path.size()},
                                                  (child_index < child_is_symlink.size() && child_is_symlink.index_unchecked(child_index)) ? true : false,
                                                  false,
                                                  false)};
            out_allowed.emplace_back(static_cast<unsigned char>(allow ? 1 : 0));
        }
    }

    // Global storage for mount roots (temporary, should be integrated with WASI implementation)
    inline ::uwvm2::utils::container::vector<mount_root_entry> mount_roots{};  // [global]

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

        mount_root_entry entry{};
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

                    scan_mode = mode_type::add_mode;
                    expecting_pattern = true;
                    ++scan_ptr;
                    continue;
                }
                if(token_str == u8"-rm")
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
                                            u8"-rm",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8" or ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                            u8"--symlink-escape-nonwasi\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
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
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        token_str,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" must follow -add or -rm or --symlink-escape-nonwasi\n\n",
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
            auto const parse_result{parse_pattern(arg_str)};
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
                entry.add_automata.emplace_back(build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::rm_mode)
            {
                entry.rm_patterns.emplace_back(arg_str);
                entry.rm_automata.emplace_back(build_nfa_from_tokens(tokens));
            }
            else if(current_mode == mode_type::symlink_escape_mode)
            {
                entry.symlink_escape_patterns.emplace_back(arg_str);
                entry.symlink_escape_automata.emplace_back(build_nfa_from_tokens(tokens));
            }
            // Note: Pattern validation already done in pre-scan, so this should not happen

            param_cursor->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
            ++param_cursor;
        }

        mount_roots.emplace_back(::std::move(entry));

        return ::uwvm2::utils::cmdline::parameter_return_type::def;
    }
}  // namespace uwvm2::uwvm::cmdline::params::details

// This cpp may not be the end of the translation unit, it may be included in other cpp files. So it needs to be pop.
// macro
#include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <uwvm2/utils/macro/pop_macros.h>

