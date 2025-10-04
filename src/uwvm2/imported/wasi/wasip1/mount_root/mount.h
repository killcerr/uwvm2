/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
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

#pragma once

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <limits>
# include <memory>
# include <new>
# include <atomic>
# include <bit>
# include <utility>
// macro
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_push_macro.h>
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/uwvm_predefine/io/impl.h>
# include <uwvm2/uwvm_predefine/utils/ansies/impl.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/debug/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::mount_root
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
        ::uwvm2::utils::container::vector<::std::size_t> start_states{};            // epsilon-closure(start)
        ::uwvm2::utils::container::u8string start_literal_prefix{};                 // maximal deterministic literal prefix
        ::uwvm2::utils::container::vector<::std::size_t> prefix_state_set_after{};  // state set after consuming the prefix
    };

    // Integer overflow-safe helpers (terminate on overflow to avoid UB)
    inline constexpr ::std::size_t checked_add_size(::std::size_t a, ::std::size_t b) noexcept
    {
        if(a > ::std::numeric_limits<::std::size_t>::max() - b) [[unlikely]] { ::fast_io::fast_terminate(); }

        return static_cast<::std::size_t>(a + b);
    }

    inline constexpr ::std::size_t checked_mul_size(::std::size_t a, ::std::size_t b) noexcept
    {
        if(a != 0uz && b != 0uz)
        {
            if(a > ::std::numeric_limits<::std::size_t>::max() / b) [[unlikely]] { ::fast_io::fast_terminate(); }
        }

        return static_cast<::std::size_t>(a * b);
    }

    inline constexpr ::std::size_t checked_add3_size(::std::size_t a, ::std::size_t b, ::std::size_t c) noexcept
    {
        return checked_add_size(checked_add_size(a, b), c);
    }

    // Generation counter bump with wrap protection for mark vectors
    inline constexpr void bump_generation(::std::size_t& gen, ::uwvm2::utils::container::vector<::std::size_t>& marks) noexcept
    {
        ++gen;
        if(gen == 0uz) [[unlikely]]
        {
            // Rare wrap-around: reset all marks to zero and restart at 1
            for(auto& m: marks) { m = 0uz; }

            gen = 1uz;
        }
    }

    /// @brief Create a new NFA state and return its index.
    inline constexpr ::std::size_t nfa_new_state(::uwvm2::utils::container::vector<nfa_state> & nfa) noexcept
    {
        auto const idx{nfa.size()};
        nfa.emplace_back(nfa_state{});
        return idx;
    }

    inline constexpr ::std::size_t nfa_new_state_unchecked(::uwvm2::utils::container::vector<nfa_state> & nfa) noexcept
    {
        auto const idx{nfa.size()};
        nfa.emplace_back_unchecked(nfa_state{});
        return idx;
    }

    /// @brief Add an epsilon transition in the NFA from -> to.
    /// @param nfa The NFA to add the epsilon transition to.
    /// @param from The source state. (unchecked)
    /// @param to The target state.
    inline constexpr void nfa_add_eps(::uwvm2::utils::container::vector<nfa_state> & nfa, ::std::size_t from, ::std::size_t to) noexcept
    {
        nfa.index_unchecked(from).eps.emplace_back(to);
    }

    inline constexpr void nfa_add_eps_unchecked(::uwvm2::utils::container::vector<nfa_state> & nfa, ::std::size_t from, ::std::size_t to) noexcept
    {
        nfa.index_unchecked(from).eps.emplace_back_unchecked(to);
    }

    /// @brief Add a consuming transition with given edge type and optional character.
    /// @param nfa The NFA to add the edge to.
    /// @param from The source state. (unchecked)
    /// @param to The target state.
    /// @param type The type of edge to add.
    /// @param ch The character to add (optional).
    inline constexpr void nfa_add_edge(::uwvm2::utils::container::vector<nfa_state> & nfa,
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
        // Compute a safe upper bound on total NFA states to avoid unchecked push overflow.
        // Components:
        // - 1 start state
        // - 1 accept state
        // - literal: size + 1
        // - ?, *, **: +1
        // - {alts}: 1 (join) + alts_count + sum(len(alt))
        ::std::size_t upper_states{2uz};
        for(auto const& t : tokens)
        {
            switch(t.type)
            {
                case pattern_token_type::literal:
                {
                    upper_states = checked_add_size(upper_states, checked_add_size(t.data.size(), 1uz));
                    break;
                }
                case pattern_token_type::question: [[fallthrough]];
                case pattern_token_type::star: [[fallthrough]];
                case pattern_token_type::double_star:
                {
                    upper_states = checked_add_size(upper_states, 1uz);
                    break;
                }
                case pattern_token_type::alternatives:
                {
                    ::std::size_t contrib{1uz}; // join node
                    contrib = checked_add_size(contrib, t.alternatives.size());
                    for(auto const& alt : t.alternatives)
                    {
                        contrib = checked_add_size(contrib, alt.size());
                    }
                    upper_states = checked_add_size(upper_states, contrib);
                    break;
                }
                [[unlikely]] default:
                {
                    // no-op
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    break;
                }
            }
        }
        automaton.nfa.reserve(upper_states);
        automaton.start = nfa_new_state_unchecked(automaton.nfa);

        // current endpoints to connect via epsilon
        ::uwvm2::utils::container::vector<::std::size_t> endpoints{};
        endpoints.reserve(checked_add_size(token_count, 1uz));
        endpoints.emplace_back_unchecked(automaton.start);  // No check is needed here since it is always greater than 1.

        bool previous_was_double_star{};
        for(auto const& token: tokens)
        {
            switch(token.type)
            {
                case pattern_token_type::literal:
                {
                    if(token.data.empty())
                    {
                        previous_was_double_star = false;
                        break;
                    }

                    // create a chain for literal
                    ::std::size_t const literal_entry_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, literal_entry_state); }

                    ::std::size_t last_state{literal_entry_state};
                    ::std::size_t optional_pre_trailing_slash_state{};
                    bool has_optional_pre_trailing_slash{};
                    if(previous_was_double_star && !token.data.empty() && token.data.front_unchecked() == u8'/')
                    {
                        auto const after_slash_state{nfa_new_state(automaton.nfa)};
                        // consume '/'
                        nfa_add_edge(automaton.nfa, last_state, after_slash_state, nfa_edge_type::char_eq, u8'/');
                        // or skip it
                        nfa_add_eps(automaton.nfa, last_state, after_slash_state);
                        last_state = after_slash_state;

                        if(token.data.size() > 1uz)
                        {
                            auto const* begin_ptr{token.data.cbegin() + 1u};
                            auto const* end_ptr{token.data.cend()};
                            for(auto const* curr_ptr{begin_ptr}; curr_ptr != end_ptr; ++curr_ptr)
                            {
                                auto const next_state{nfa_new_state(automaton.nfa)};
                                nfa_add_edge(automaton.nfa, last_state, next_state, nfa_edge_type::char_eq, *curr_ptr);
                                last_state = next_state;
                            }
                        }
                    }
                    else
                    {
                        ::std::size_t pre_trailing_slash_state{};
                        bool has_trailing_slash{};
                        auto const* begin_ptr{token.data.cbegin()};
                        auto const* end_ptr{token.data.cend()};
                        for(auto const* curr_ptr{begin_ptr}; curr_ptr != end_ptr; ++curr_ptr)
                        {
                            auto const literal_char{*curr_ptr};
                            if(curr_ptr + 1u == end_ptr && literal_char == u8'/')
                            {
                                pre_trailing_slash_state = last_state;
                                has_trailing_slash = true;
                            }
                            auto const next_state{nfa_new_state(automaton.nfa)};
                            nfa_add_edge(automaton.nfa, last_state, next_state, nfa_edge_type::char_eq, literal_char);
                            last_state = next_state;
                        }
                        if(has_trailing_slash)
                        {
                            // allow skipping the trailing '/'
                            optional_pre_trailing_slash_state = pre_trailing_slash_state;
                            has_optional_pre_trailing_slash = true;
                        }
                    }

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(last_state);
                    if(has_optional_pre_trailing_slash) { endpoints.emplace_back_unchecked(optional_pre_trailing_slash_state); }

                    previous_was_double_star = false;
                    break;
                }
                case pattern_token_type::question:
                {
                    ::std::size_t next_state{nfa_new_state(automaton.nfa)};
                    for(auto const endpoint_state: endpoints) { nfa_add_edge(automaton.nfa, endpoint_state, next_state, nfa_edge_type::any_not_slash); }

                    endpoints.clear();
                    endpoints.emplace_back_unchecked(next_state);

                    previous_was_double_star = false;
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

                    previous_was_double_star = false;
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

                    previous_was_double_star = true;
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

                    previous_was_double_star = false;
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    previous_was_double_star = false;
                    break;
                }
            }
        }

        // accepting tail
        ::std::size_t const accept_state{nfa_new_state(automaton.nfa)};
        automaton.nfa.index_unchecked(accept_state).accepting = true;
        for(auto const endpoint_state: endpoints) { nfa_add_eps(automaton.nfa, endpoint_state, accept_state); }

        // finalize: precompute epsilon-closures and build epsilon-free transitions
        // 1) compute closure for each state
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::vector<::std::size_t>> closures{};
        closures.resize(automaton.nfa.size());

        ::uwvm2::utils::container::vector<::std::size_t> stack{};
        ::uwvm2::utils::container::vector<::std::size_t> seen_gen{};
        ::std::size_t seen_cur{};
        seen_gen.resize(automaton.nfa.size());

        auto compute_closure{[&](::std::size_t sidx) constexpr noexcept
                             {
                                 auto& out{closures.index_unchecked(sidx)};
                                 if(!out.empty()) { return; }
                                 bump_generation(seen_cur, seen_gen);
                                 stack.clear();
                                 stack.emplace_back(sidx);
                                 seen_gen.index_unchecked(sidx) = seen_cur;
                                 out.clear();
                                 out.reserve(8uz);
                                 out.emplace_back(sidx);
                                 while(!stack.empty())
                                 {
                                     auto curr{stack.back_unchecked()};
                                     stack.pop_back_unchecked();
                                     auto const& st{automaton.nfa.index_unchecked(curr)};
                                     for(auto const to: st.eps)
                                     {
                                         if(seen_gen.index_unchecked(to) != seen_cur)
                                         {
                                             seen_gen.index_unchecked(to) = seen_cur;
                                             stack.emplace_back(to);
                                             out.emplace_back(to);
                                         }
                                     }
                                 }
                             }};

        for(::std::size_t i{}; i != automaton.nfa.size(); ++i) { compute_closure(i); }

        // 2) propagate accepting via closure
        auto closures_curr{closures.begin()};
        for(auto& st: automaton.nfa)
        {
            if(st.accepting) { continue; }
            bool acc{};

            for(auto const cst: *closures_curr)
            {
                if(automaton.nfa.index_unchecked(cst).accepting)
                {
                    acc = true;
                    break;
                }
            }
            ++closures_curr;

            if(acc) { st.accepting = true; }
        }

        // 3) build epsilon-free edges: for each state, gather edges of all states in its closure, and push edges to closure(dest)
        closures_curr = closures.begin();
        for(auto& st: automaton.nfa)
        {
            ::uwvm2::utils::container::vector<nfa_edge> new_edges{};
            // estimate size
            new_edges.reserve(checked_add_size(st.edges.size(), 4uz));
            for(auto const src_in_closure: *closures_curr)
            {
                auto const& src{automaton.nfa.index_unchecked(src_in_closure)};
                for(auto const& e: src.edges)
                {
                    // expand to closure of e.to
                    for(auto const to_state: closures.index_unchecked(e.to)) { new_edges.emplace_back(nfa_edge{.type = e.type, .ch = e.ch, .to = to_state}); }
                }
            }
            ++closures_curr;

            st.edges.swap(new_edges);
            // strip eps to reduce footprint (not used at runtime anymore)
            st.eps.clear();
        }

        // 4) compute start_states as closure(start)
        automaton.start_states = closures.index_unchecked(automaton.start);

        // 5) compute maximal deterministic literal prefix from start_states
        {
            ::uwvm2::utils::container::vector<::std::size_t> curr_set{automaton.start_states};
            ::uwvm2::utils::container::vector<::std::size_t> next_set{};
            ::uwvm2::utils::container::vector<::std::size_t> visit{};
            ::std::size_t gen{};

            automaton.start_literal_prefix.clear();
            bool can_extend{true};
            while(can_extend && !curr_set.empty())
            {
                // Find common literal char across all states; allow multiple edges per state if all char_eq and same ch
                bool first_state{true};
                char8_t common_ch{};
                for(auto const sidx: curr_set)
                {
                    auto const& st{automaton.nfa.index_unchecked(sidx)};
                    if(st.edges.empty())
                    {
                        can_extend = false;
                        break;
                    }
                    // check all edges char_eq and if they share one same ch
                    char8_t local_ch{};
                    bool local_set{false};
                    for(auto const& e: st.edges)
                    {
                        if(e.type != nfa_edge_type::char_eq)
                        {
                            can_extend = false;
                            break;
                        }
                        if(!local_set)
                        {
                            local_ch = e.ch;
                            local_set = true;
                        }
                        else if(e.ch != local_ch)
                        {
                            can_extend = false;
                            break;
                        }
                    }
                    if(!can_extend) { break; }
                    if(first_state)
                    {
                        common_ch = local_ch;
                        first_state = false;
                    }
                    else if(local_ch != common_ch)
                    {
                        can_extend = false;
                        break;
                    }
                }

                if(!can_extend) { break; }

                // build next_set (union of destinations for common_ch)
                next_set.clear();
                if(visit.size() < automaton.nfa.size()) { visit.resize(automaton.nfa.size()); }
                bump_generation(gen, visit);
                for(auto const sidx: curr_set)
                {
                    auto const& st{automaton.nfa.index_unchecked(sidx)};
                    for(auto const& e: st.edges)
                    {
                        if(e.ch == common_ch)
                        {
                            if(visit.index_unchecked(e.to) != gen)
                            {
                                visit.index_unchecked(e.to) = gen;
                                next_set.emplace_back(e.to);
                            }
                        }
                    }
                }

                if(next_set.empty()) { break; }
                automaton.start_literal_prefix.push_back(common_ch);
                curr_set.swap(next_set);
            }

            automaton.prefix_state_set_after = ::std::move(curr_set);
        }

        return automaton;
    }

    /// @brief Expand a set of NFA states by following epsilon transitions (in-place closure).
    // nfa_epsilon_closure no longer used at runtime after finalize

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
        // Fast literal prefix check
        ::std::size_t pos{};
        if(!automaton.start_literal_prefix.empty())
        {
            auto const& pref{automaton.start_literal_prefix};
            if(path.size() < pref.size()) { return false; }
            bool ok{true};
            for(::std::size_t i{}; i < pref.size(); ++i)
            {
                if(path.index_unchecked(i) != pref.index_unchecked(i))
                {
                    ok = false;
                    break;
                }
            }
            if(!ok) { return false; }
            current_state_set = automaton.prefix_state_set_after;
            pos = pref.size();
        }
        else
        {
            current_state_set = automaton.start_states;  // already epsilon-closed
            pos = 0uz;
        }

        ::uwvm2::utils::container::vector<::std::size_t> next_state_set{};
        ::uwvm2::utils::container::vector<::std::size_t> visit_gen{};
        ::std::size_t gen{};

        for(; pos < path.size(); ++pos)
        {
            next_state_set.clear();
            next_state_set.reserve(checked_add_size(current_state_set.size(), 4uz));

            if(visit_gen.size() < automaton.nfa.size()) { visit_gen.resize(automaton.nfa.size()); }
            bump_generation(gen, visit_gen);

            for(auto const state_index: current_state_set)
            {
                auto const& state{automaton.nfa.index_unchecked(state_index)};
                for(auto const& edge: state.edges)
                {
                    if(nfa_edge_match(edge, path.index_unchecked(pos)))
                    {
                        if(visit_gen.index_unchecked(edge.to) != gen)
                        {
                            visit_gen.index_unchecked(edge.to) = gen;
                            next_state_set.emplace_back(edge.to);
                        }
                    }
                }
            }

            if(next_state_set.empty()) { return false; }
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

            // Unmatched '}' at top-level is invalid
            if(current_char == u8'}')
            {
                return {
                    pattern_match_error{.has_error = true, .error_pos = pattern_index, .error_message = u8"Unmatched '}', missing opening '{'"},
                    ::std::move(tokens)
                };
            }

            // Handle escape sequences
            if(current_char == u8'\\' && pattern_index + 1uz < pattern.size())
            {
                char8_t const escaped_char{pattern.index_unchecked(pattern_index + 1uz)};  // safe: pattern_index+1 < pattern.size()
                current_literal.push_back(escaped_char);
                pattern_index += 2uz;
                continue;
            }
            if(current_char == u8'\\' && pattern_index + 1uz >= pattern.size())
            {
                return {
                    pattern_match_error{.has_error = true, .error_pos = pattern_index, .error_message = u8"Trailing escape character '\\'"},
                    ::std::move(tokens)
                };
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
                ::std::size_t const alt_count{checked_add_size(top_level_commas, 1uz)};

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
        // The purpose of using `string_view` here is because all data is provided via the command line and persists throughout the program's execution.
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> add_patterns{};             // whitelist
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> rm_patterns{};              // blacklist
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> symlink_escape_patterns{};  // parameter patterns

        // wasi create lifecycle whitelist (owns keys, queryable by u8string_view)
        ::uwvm2::utils::container::unordered_flat_set<::uwvm2::utils::container::u8string,
                                                      ::uwvm2::utils::container::pred::u8string_view_hash,
                                                      ::uwvm2::utils::container::pred::u8string_view_equal>
            created_by_wasi_set{};

        // Compiled automata for fast matching at runtime (readdir/path access)
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> add_automata{};
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> rm_automata{};
        ::uwvm2::utils::container::vector<compiled_pattern_automaton> symlink_escape_automata{};
    };

    /// @brief Record a path created by WASI into lifecycle whitelist set.
    inline constexpr void wasi_mark_created(mount_root_entry & entry, ::uwvm2::utils::container::u8string_view canonical_relative_path) noexcept
    {
        entry.created_by_wasi_set.emplace(canonical_relative_path.cbegin(), canonical_relative_path.cend());
    }

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
                                                        bool is_symlink_creation,
                                                        ::uwvm2::utils::container::u8string_view symlink_target_canonical_rel = {}) noexcept
    {
        // A) symlink-escape-nonwasi: highest priority
        if(is_symlink)
        {
            if(match_any(relative_path, entry.symlink_escape_automata)) { return access_policy::allow_bypass_root; }
        }

        if(is_symlink_creation) [[unlikely]]
        {
            if(match_any(relative_path, entry.symlink_escape_automata)) { return access_policy::deny; }
        }

        // A.1) lifecycle whitelist: if the resolved symlink target (or the path itself when non-symlink) was created by WASI, allow
        if(!symlink_target_canonical_rel.empty())
        {
            // Query lifecycle whitelist via transparent lookup (u8string_view, zero allocation)
            if(entry.created_by_wasi_set.contains(symlink_target_canonical_rel)) { return access_policy::allow; }
        }
        else if(is_wasi_created)
        {
            // direct object marked as WASI-created
            return access_policy::allow;
        }

        // B) WASI whitelist: handled by WASI, but when informed, it should override blacklist
        // (already handled above)

        // C) whitelist: if present and matched, allow (non-restrictive otherwise)
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
                                               access_policy* out_policy = nullptr,
                                               ::uwvm2::utils::container::u8string_view symlink_target_canonical_rel = {}) noexcept
    {
        auto const pol{evaluate_path_access(entry, relative_path, is_symlink, is_wasi_created, is_symlink_creation, symlink_target_canonical_rel)};
        if(out_policy) { *out_policy = pol; }
        return pol != access_policy::deny;
    }

    /// @brief Filter a single directory layer's child entries according to mount rules.
    inline constexpr void wasi_filter_directory_entries(mount_root_entry const& entry,
                                                        ::uwvm2::utils::container::u8string_view relative_dir,
                                                        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> const& child_names,
                                                        ::uwvm2::utils::container::vector<bool> const& child_is_symlink,
                                                        ::uwvm2::utils::container::vector<bool>& out_allowed) noexcept
    {
        out_allowed.clear();
        out_allowed.reserve(child_names.size());

        bool const dir_has_trailing_slash{!relative_dir.empty() && relative_dir.back_unchecked() == u8'/'};  // back_unchecked safe: checked empty
        ::uwvm2::utils::container::u8string base_prefix{};
        if(!relative_dir.empty())
        {
            base_prefix.reserve(checked_add_size(relative_dir.size(), static_cast<::std::size_t>(!dir_has_trailing_slash)));
            base_prefix.append(relative_dir);
            if(!dir_has_trailing_slash) { base_prefix.push_back_unchecked(u8'/'); }
        }
        ::uwvm2::utils::container::u8string relative_path{base_prefix};
        ::std::size_t const prefix_len{relative_path.size()};
        for(::std::size_t child_index{}; child_index != child_names.size(); ++child_index)
        {
            auto const child_name{child_names.index_unchecked(child_index)};  // index_unchecked safe: child_index < child_names.size()

            // Build relative path: relative_dir + ['/' if needed] + name
            relative_path.resize(prefix_len);
            relative_path.reserve(checked_add_size(prefix_len, child_name.size()));
            relative_path.append(child_name);

            auto const rel_view{
                ::uwvm2::utils::container::u8string_view{relative_path.data(), relative_path.size()}
            };

            bool allowed{};
            if(!entry.add_automata.empty())
            {
                // Strict whitelist for directory listing: must match add_automata
                allowed = match_any(rel_view, entry.add_automata);
                if(allowed)
                {
                    allowed = wasi_rule_allow_open(entry,
                                                   rel_view,
                                                   (child_index < child_is_symlink.size() && child_is_symlink.index_unchecked(child_index)),
                                                   false,
                                                   false);
                }
            }
            else
            {
                allowed = wasi_rule_allow_open(entry,
                                               rel_view,
                                               (child_index < child_is_symlink.size() && child_is_symlink.index_unchecked(child_index)),
                                               false,
                                               false);
            }

            out_allowed.emplace_back_unchecked(allowed);
        }
    }

}  // namespace uwvm2::imported::wasi::wasip1::mount_root

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_pop_macro.h>
#endif
