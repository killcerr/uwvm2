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
    inline ::std::size_t nfa_new_state(::uwvm2::utils::container::vector<nfa_state>& nfa) noexcept
    {
        auto const idx{nfa.size()};
        nfa.emplace_back(nfa_state{});
        return idx;
    }

    /// @brief Add an epsilon transition in the NFA from -> to.
    inline void nfa_add_eps(::uwvm2::utils::container::vector<nfa_state>& nfa, ::std::size_t from, ::std::size_t to) noexcept
    {
        nfa.index_unchecked(from).eps.emplace_back(to);
    }

    /// @brief Add a consuming transition with given edge type and optional character.
    inline void
        nfa_add_edge(::uwvm2::utils::container::vector<nfa_state>& nfa, ::std::size_t from, ::std::size_t to, nfa_edge_type type, char8_t ch = u8'\0') noexcept
    {
        nfa.index_unchecked(from).edges.emplace_back(nfa_edge{.type = type, .ch = ch, .to = to});
    }

    // Build an NFA from parsed pattern tokens
    /// @brief Compile a parsed token sequence into an NFA implementing glob semantics.
    inline compiled_pattern_automaton build_nfa_from_tokens(::uwvm2::utils::container::vector<pattern_token> const& tokens) noexcept
    {
        compiled_pattern_automaton a{};
        a.nfa.reserve(1uz + tokens.size() * 4uz);
        a.start = nfa_new_state(a.nfa);

        // current endpoints to connect via epsilon
        ::uwvm2::utils::container::vector<::std::size_t> endpoints{};
        endpoints.reserve(tokens.size() + 1uz);
        endpoints.emplace_back(a.start);

        for(auto const& tok: tokens)
        {
            switch(tok.type)
            {
                case pattern_token_type::literal:
                {
                    if(tok.data.empty()) { break; }

                    // create a chain for literal
                    ::std::size_t entry{nfa_new_state(a.nfa)};
                    for(auto const ep: endpoints) { nfa_add_eps(a.nfa, ep, entry); }

                    ::std::size_t last{entry};
                    for(::std::size_t i{}; i < tok.data.size(); ++i)
                    {
                        auto const ch{tok.data.index_unchecked(i)};  // index_unchecked is safe: i < tok.data.size()
                        auto const next{nfa_new_state(a.nfa)};
                        nfa_add_edge(a.nfa, last, next, nfa_edge_type::char_eq, ch);
                        last = next;
                    }

                    endpoints.clear();
                    endpoints.emplace_back(last);
                    break;
                }
                case pattern_token_type::question:
                {
                    ::std::size_t next{nfa_new_state(a.nfa)};
                    for(auto const ep: endpoints) { nfa_add_edge(a.nfa, ep, next, nfa_edge_type::any_not_slash); }
                    endpoints.clear();
                    endpoints.emplace_back(next);
                    break;
                }
                case pattern_token_type::star:
                {
                    // zero or more of non-slash
                    ::std::size_t s{nfa_new_state(a.nfa)};
                    for(auto const ep: endpoints) { nfa_add_eps(a.nfa, ep, s); }
                    nfa_add_edge(a.nfa, s, s, nfa_edge_type::any_not_slash);
                    endpoints.clear();
                    endpoints.emplace_back(s);
                    break;
                }
                case pattern_token_type::double_star:
                {
                    // zero or more of any char
                    ::std::size_t s{nfa_new_state(a.nfa)};
                    for(auto const ep: endpoints) { nfa_add_eps(a.nfa, ep, s); }
                    nfa_add_edge(a.nfa, s, s, nfa_edge_type::any_char);
                    endpoints.clear();
                    endpoints.emplace_back(s);
                    break;
                }
                case pattern_token_type::alternatives:
                {
                    ::std::size_t join{nfa_new_state(a.nfa)};
                    for(auto const& alt: tok.alternatives)
                    {
                        ::std::size_t entry{nfa_new_state(a.nfa)};
                        for(auto const ep: endpoints) { nfa_add_eps(a.nfa, ep, entry); }
                        ::std::size_t last{entry};
                        for(::std::size_t i{}; i < alt.size(); ++i)
                        {
                            auto const ch{alt.index_unchecked(i)};  // safe: i < alt.size()
                            auto const next{nfa_new_state(a.nfa)};
                            nfa_add_edge(a.nfa, last, next, nfa_edge_type::char_eq, ch);
                            last = next;
                        }
                        nfa_add_eps(a.nfa, last, join);
                    }
                    endpoints.clear();
                    endpoints.emplace_back(join);
                    break;
                }
                default: break;
            }
        }

        // accepting tail
        ::std::size_t accept{nfa_new_state(a.nfa)};
        a.nfa.index_unchecked(accept).accepting = true;
        for(auto const ep: endpoints) { nfa_add_eps(a.nfa, ep, accept); }

        return a;
    }

    /// @brief Expand a set of NFA states by following epsilon transitions (in-place closure).
    inline void nfa_epsilon_closure(::uwvm2::utils::container::vector<nfa_state> const& nfa, ::uwvm2::utils::container::vector<::std::size_t>& set) noexcept
    {
        ::uwvm2::utils::container::vector<::std::size_t> stack{};
        stack.reserve(set.size());
        for(auto const s: set) { stack.emplace_back(s); }

        ::uwvm2::utils::container::vector<unsigned char> vis{};
        vis.resize(nfa.size());
        for(auto const s: set) { vis.index_unchecked(s) = static_cast<unsigned char>(1); }

        while(!stack.empty())
        {
            auto const s{stack.back()};
            stack.pop_back();
            auto const& st{nfa.index_unchecked(s)};
            for(auto const to: st.eps)
            {
                if(!vis.index_unchecked(to))
                {
                    vis.index_unchecked(to) = static_cast<unsigned char>(1);
                    set.emplace_back(to);
                    stack.emplace_back(to);
                }
            }
        }
    }

    /// @brief Check if a consuming edge can match a given character.
    inline bool nfa_edge_match(nfa_edge const& e, char8_t ch) noexcept
    {
        switch(e.type)
        {
            case nfa_edge_type::char_eq: return e.ch == ch;
            case nfa_edge_type::any_not_slash: return ch != u8'/';
            case nfa_edge_type::any_char: return true;
            default: return false;
        }
    }

    /// @brief Match a full path string against a compiled NFA.
    inline bool match_nfa(compiled_pattern_automaton const& a, ::uwvm2::utils::container::u8string_view path) noexcept
    {
        ::uwvm2::utils::container::vector<::std::size_t> curr{};
        curr.emplace_back(a.start);
        nfa_epsilon_closure(a.nfa, curr);

        for(::std::size_t i{}; i < path.size(); ++i)
        {
            auto const ch{path.index_unchecked(i)};
            ::uwvm2::utils::container::vector<::std::size_t> next{};
            next.reserve(curr.size() + 4uz);

            ::uwvm2::utils::container::vector<unsigned char> vis{};
            vis.resize(a.nfa.size());

            for(auto const s: curr)
            {
                auto const& st{a.nfa.index_unchecked(s)};
                for(auto const& e: st.edges)
                {
                    if(nfa_edge_match(e, ch))
                    {
                        if(!vis.index_unchecked(e.to))
                        {
                            vis.index_unchecked(e.to) = static_cast<unsigned char>(1);
                            next.emplace_back(e.to);
                        }
                    }
                }
            }

            if(next.empty()) { return false; }
            nfa_epsilon_closure(a.nfa, next);
            curr.swap(next);
        }

        for(auto const s: curr)
        {
            if(a.nfa.index_unchecked(s).accepting) { return true; }
        }
        return false;
    }

    // Parse pattern into tokens
    /// @brief Tokenize a glob pattern; validates syntax and produces a token sequence.
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
                                   tokens.emplace_back(pattern_token{.type = pattern_token_type::literal, .data = ::std::move(current_literal)});
                                   current_literal = {};
                                   current_literal.reserve(pattern_size);
                               }
                           }};

        while(i < pattern.size())
        {
            char8_t const ch{pattern.index_unchecked(i)};  // safe: i < pattern.size()

            // Handle escape sequences
            if(ch == u8'\\' && i + 1uz < pattern.size())
            {
                char8_t const next{pattern.index_unchecked(i + 1uz)};  // safe: i+1 < pattern.size()
                current_literal.push_back(next);
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
                    tokens.emplace_back(pattern_token{.type = pattern_token_type::double_star});
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
                tokens.emplace_back(pattern_token{.type = pattern_token_type::star});
                ++i;
                continue;
            }

            // Handle ?
            if(ch == u8'?')
            {
                flush_literal();
                tokens.emplace_back(pattern_token{.type = pattern_token_type::question});
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
                    char8_t const ch2{pattern.index_unchecked(j)};  // safe: j < pattern.size()

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
                    char8_t const ch2{pattern.index_unchecked(k)};  // safe: k < content_end <= pattern.size()

                    if(ch2 == u8'\\' && k + 1uz < content_end)
                    {
                        current_alt.push_back(pattern.index_unchecked(k + 1uz));
                        k += 2uz;
                        continue;
                    }

                    if(ch2 == u8'{')
                    {
                        ++inner_depth;
                        current_alt.push_back(ch2);
                        ++k;
                        continue;
                    }
                    else if(ch2 == u8'}')
                    {
                        --inner_depth;
                        if(inner_depth >= 1uz) { current_alt.push_back(ch2); }
                        ++k;
                        continue;
                    }
                    else if(ch2 == u8',' && inner_depth == 1uz)
                    {
                        alternatives.emplace_back(::std::move(current_alt));
                        current_alt = {};
                        current_alt.reserve(content_end - content_begin);
                        ++k;
                        continue;
                    }

                    current_alt.push_back(ch2);
                    ++k;
                }

                alternatives.emplace_back(::std::move(current_alt));

                pattern_token tok{.type = pattern_token_type::alternatives, .alternatives = ::std::move(alternatives)};
                tokens.emplace_back(::std::move(tok));
                i = content_end + 1uz;
                continue;
            }

            // Regular character
            current_literal.push_back(ch);
            ++i;
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
    inline bool match_compiled_pattern(::uwvm2::utils::container::u8string_view path, compiled_pattern_automaton const& a) noexcept
    {
        return match_nfa(a, path);
    }

    /// @brief Match a path against any of the compiled automata in the list.
    inline bool match_any(::uwvm2::utils::container::u8string_view path, ::uwvm2::utils::container::vector<compiled_pattern_automaton> const& automata) noexcept
    {
        for(auto const& a: automata)
        {
            if(match_compiled_pattern(path, a)) { return true; }
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
    inline access_policy evaluate_path_access(mount_root_entry const& entry,
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
    inline bool wasi_rule_allow_open(mount_root_entry const& entry,
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
    inline void wasi_filter_directory_entries(mount_root_entry const& entry,
                                              ::uwvm2::utils::container::u8string_view relative_dir,
                                              ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> const& child_names,
                                              ::uwvm2::utils::container::vector<unsigned char> const& child_is_symlink,
                                              ::uwvm2::utils::container::vector<unsigned char>& out_allowed) noexcept
    {
        out_allowed.clear();
        out_allowed.reserve(child_names.size());

        bool const dir_has_trailing_slash{!relative_dir.empty() && relative_dir.back_unchecked() == u8'/'};  // back_unchecked safe: checked empty
        for(::std::size_t i{}; i < child_names.size(); ++i)
        {
            auto const name{child_names.index_unchecked(i)};  // index_unchecked safe: i < child_names.size()

            // Build relative path: relative_dir + ['/' if needed] + name
            ::uwvm2::utils::container::u8string rel_path{};
            rel_path.reserve(relative_dir.size() + (dir_has_trailing_slash ? 0uz : 1uz) + name.size());
            if(!relative_dir.empty())
            {
                rel_path.append(relative_dir);
                if(!dir_has_trailing_slash) { rel_path.push_back(u8'/'); }
            }
            rel_path.append(name);

            auto const allow{wasi_rule_allow_open(entry,
                                                  ::uwvm2::utils::container::u8string_view{rel_path.data(), rel_path.size()},
                                                  (i < child_is_symlink.size() && child_is_symlink.index_unchecked(i)) ? true : false,
                                                  false,
                                                  false)};
            out_allowed.emplace_back(static_cast<unsigned char>(allow ? 1 : 0));
        }
    }

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

                        if(v2 == u8"-add" || v2 == u8"-rm" || v2 == u8"--symlink-escape-nonwasi")
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
                                    if(v3 == u8"-add" || v3 == u8"-rm" || v3 == u8"--symlink-escape-nonwasi") { break; }
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
                    ++scanp;
                    continue;
                }
                if(s == u8"--symlink-escape-nonwasi")
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
                                        s,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" must follow -add or -rm or --symlink-escape-nonwasi\n\n",
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
            else if(arg_str == u8"--symlink-escape-nonwasi")
            {
                current_mode = mode_type::symlink_escape_mode;
                currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;
                ++currp1;
                continue;
            }

            // Compile pattern tokens and build NFA (validate at the same time)
            auto const pr{parse_pattern(arg_str)};
            auto const& err{pr.error};
            auto const& tokens{pr.tokens};

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

