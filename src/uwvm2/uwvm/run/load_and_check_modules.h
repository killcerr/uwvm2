/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-27
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
# include <type_traits>
# include <utility>
# include <memory>
# include <limits>
# include <algorithm>
// macro
# include <uwvm2/utils/macro/push_macros.h>
# include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/utils/ansies/impl.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/utils/madvise/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/impl.h>
# include <uwvm2/parser/wasm/binfmt/base/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/utils/memory/impl.h>
# include <uwvm2/uwvm/utils/depend/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
# include <uwvm2/uwvm/wasm/impl.h>
# include "retval.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::uwvm::run
{
    /// @brief Detect all cycles in dependency graph (deduplicated and efficient).
    ///        Based on "SCC decomposition + ordered DFS (Johnson's idea)": first decompose the graph into strongly connected components,
    ///        then perform restricted DFS within each SCC, only starting enumeration from the minimum id vertex on the cycle,
    ///        thus avoiding rotation duplicates like A->B->A and B->A->B.
    /// @param adjacency_list The adjacency list of the dependency graph
    /// @return All unique cycles (each cycle has same start and end: v0 -> ... -> v0)
    inline constexpr auto detect_cycles(
        ::uwvm2::utils::container::unordered_flat_map<::uwvm2::utils::container::u8string_view,
                                                      ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view>> const&
            adjacency_list) noexcept
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer detect_cycles_timer{u8"detect cycles"};
#endif

        using module_name_t = ::uwvm2::utils::container::u8string_view;
        using cycle_t = ::uwvm2::utils::container::vector<module_name_t>;
        using cycles_t = ::uwvm2::utils::container::vector<cycle_t>;

        auto const curr_recursion_depth_limit{::uwvm2::uwvm::utils::depend::recursion_depth_limit};

        cycles_t all_cycles{};
        if(adjacency_list.empty()) [[unlikely]] { return all_cycles; }

        // Check dependency count limit
        // The number of recursions is greater than or equal to the number of dependencies, so it can be checked in advance.
        if(auto const dependency_count{adjacency_list.size()}; dependency_count > curr_recursion_depth_limit && curr_recursion_depth_limit != 0uz)
        {
            // Output warning about dependency count exceeding limit
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[warn]  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Dependency count \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                dependency_count,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" exceeds limit \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                curr_recursion_depth_limit,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Dependency check skipped. Use \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--wasm-depend-recursion-limit",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" to force check. ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                u8"(depend)\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

            if(::uwvm2::uwvm::io::depend_warning_fatal) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                    u8"[fatal] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Convert warnings to fatal errors. ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                    u8"(depend)\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                ::fast_io::fast_terminate();
            }

            return all_cycles;
        }

        // 1) Compress names to integer ids for efficient computation (avoid frequent hashing of string_view)
        // unordered_flat_map use xxh3 hash
        ::uwvm2::utils::container::unordered_flat_map<module_name_t, ::std::size_t> name_to_id{};
        ::uwvm2::utils::container::vector<module_name_t> id_to_name{};
        auto const adjacency_list_size{adjacency_list.size()};
        id_to_name.reserve(adjacency_list_size);
        name_to_id.reserve(adjacency_list_size);
        // id_to_name, name_to_id are all of size adjacency_list_size
        for(auto const& [name, _]: adjacency_list)
        {
            auto const id{id_to_name.size()};
            // Safe: we just reserved space for id_to_name.size() elements
            id_to_name.push_back_unchecked(name);
            name_to_id.emplace(name, id);
        }

        // 2) Build compact adjacency list (by id)
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::vector<::std::size_t>> graph{};
        graph.resize(id_to_name.size());
        for(auto const& [u_name, outs]: adjacency_list)
        {
            auto const u_id{name_to_id[u_name]};
            // Safe: u_id is guaranteed to be < graph.size() because we just built name_to_id
            auto& u_out{graph.index_unchecked(u_id)};
            u_out.reserve(outs.size());
            for(auto const v_name: outs)
            {
                auto const it{name_to_id.find(v_name)};
                if(it != name_to_id.end())
                {
                    // Safe: we just reserved space for outs.size() elements
                    u_out.push_back_unchecked(it->second);
                }
            }
        }

        // 3) Tarjan's algorithm to find strongly connected components (O(V+E))
        // Use std::make_signed_t<std::size_t> for index/low to ensure type compatibility with size_t
        // Check for overflow when graph size exceeds signed type maximum
        using signed_size_t = ::std::make_signed_t<::std::size_t>;
        constexpr auto signed_size_max{::std::numeric_limits<signed_size_t>::max()};

        auto const graph_size{graph.size()};

        if(graph_size > static_cast<::std::size_t>(signed_size_max)) [[unlikely]]
        {
            // Output warning about cycle detection being impossible due to graph size
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[warn]  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Graph size \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                graph_size,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" exceeds maximum signed size \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_GREEN),
                                signed_size_max,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Cycle detection skipped. ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                u8"(depend)\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

            if(::uwvm2::uwvm::io::depend_warning_fatal) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                    u8"[fatal] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Convert warnings to fatal errors. ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                    u8"(depend)\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                ::fast_io::fast_terminate();
            }

            return all_cycles;
        }

        ::uwvm2::utils::container::vector<signed_size_t> index(graph_size, static_cast<signed_size_t>(-1));
        ::uwvm2::utils::container::vector<signed_size_t> low(graph_size, signed_size_t{});
        ::uwvm2::utils::container::vector<bool> on_stack(graph_size, false);
        ::uwvm2::utils::container::vector<::std::size_t> st{};
        st.reserve(graph_size);
        // index, low, on_stack, st are all of size graph_size

        // The reason for using size_t and converting it to signed_size_t is that the maximum number of times this function can be called is signed_size_t_max.
        // When dfs_idx++ is called later, even if the value before the UB occurred is used, UB still cannot be avoided. Therefore, unsigned storage is used
        // here to ensure that UB will never occur during subsequent increments, while also ensuring that all returns are signed. Converting unsigned to signed
        // also does not cause any UB.

        ::std::size_t dfs_idx{};

        ::uwvm2::utils::container::vector<::uwvm2::utils::container::vector<::std::size_t>> sccs{};

        for(::std::size_t v{}; v != graph_size; ++v)
        {
            // Safe: v is guaranteed to be < graph.size() because we iterate over graph.size()
            if(index.index_unchecked(v) != static_cast<signed_size_t>(-1)) { continue; }

            // Iterative implementation to avoid stack overflow with recursion depth limit
            struct stack_frame_t
            {
                ::std::size_t v;
                ::std::size_t neighbor_idx;
                bool is_initial_call;
                ::std::size_t depth;

                inline constexpr stack_frame_t(::std::size_t vertex, ::std::size_t current_depth, bool initial = true) noexcept :
                    v{vertex}, neighbor_idx{0uz}, is_initial_call{initial}, depth{current_depth}
                {
                }
            };

            // Use explicit stack to avoid recursion with depth limit
            ::uwvm2::utils::container::vector<stack_frame_t> call_stack{};
            call_stack.reserve(graph_size);  // Reserve maximum possible depth, so we can use push_back_unchecked

            // size of graph_size != 0
            call_stack.emplace_back_unchecked(v, 0uz, true);

            while(!call_stack.empty()) [[likely]]
            {
                auto& frame{call_stack.back_unchecked()};

                // Check recursion depth limit
                if(frame.depth > curr_recursion_depth_limit && curr_recursion_depth_limit != 0uz) [[unlikely]]
                {
                    // Output warning about recursion depth limit exceeded
                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        u8"[warn]  ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Recursion depth limit \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        curr_recursion_depth_limit,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" exceeded at vertex \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        frame.v,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\". Skipping further checks. Use \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                        u8"--wasm-depend-recursion-limit",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" to force check. ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                        u8"(depend)\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                    if(::uwvm2::uwvm::io::depend_warning_fatal) [[unlikely]]
                    {
                        ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                            u8"uwvm: ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                            u8"[fatal] ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"Convert warnings to fatal errors. ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                            u8"(depend)\n\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                        ::fast_io::fast_terminate();
                    }

                    // Pop this frame and continue with next
                    // But first, update parent's low value if this is not the root call
                    if(call_stack.size() > 1uz) [[likely]]
                    {
                        auto& parent_frame{call_stack.index_unchecked(call_stack.size() - 2uz)};
                        low.index_unchecked(parent_frame.v) = ::std::min(low.index_unchecked(parent_frame.v), low.index_unchecked(frame.v));
                    }

                    call_stack.pop_back_unchecked();
                    break;
                }

                if(frame.is_initial_call) [[unlikely]]
                {
                    // Initial call - set up the vertex

                    // The reason for using size_t and converting it to signed_size_t is that the maximum number of times this function can be called is
                    // signed_size_t_max. When dfs_idx++ is called later, even if the value before the UB occurred is used, UB still cannot be avoided.
                    // Therefore, unsigned storage is used here to ensure that UB will never occur during subsequent increments, while also ensuring that all
                    // returns are signed. Converting unsigned to signed also does not cause any UB.

                    index.index_unchecked(frame.v) = low.index_unchecked(frame.v) = static_cast<signed_size_t>(dfs_idx++);
                    // st size < graph_size, so push_back_unchecked is safe
                    st.push_back_unchecked(frame.v);
                    on_stack.index_unchecked(frame.v) = true;
                    frame.is_initial_call = false;
                }

                // Process neighbors
                auto const& neighbors{graph.index_unchecked(frame.v)};
                if(frame.neighbor_idx < neighbors.size()) [[likely]]
                {
                    auto const w{neighbors.index_unchecked(frame.neighbor_idx)};
                    ++frame.neighbor_idx;

                    // Safe: w is guaranteed to be < graph.size() because it comes from graph[v]
                    if(index.index_unchecked(w) == static_cast<signed_size_t>(-1))
                    {
                        // Recursive call - push new frame with increased depth
                        // size_t operations never wrap around, because the maximum size is signed size_t max.
                        call_stack.emplace_back_unchecked(w, frame.depth + 1uz, true);
                    }
                    else if(on_stack.index_unchecked(w)) { low.index_unchecked(frame.v) = ::std::min(low.index_unchecked(frame.v), index.index_unchecked(w)); }
                }
                else
                {
                    // All neighbors processed - check if this is a root of SCC
                    if(low.index_unchecked(frame.v) == index.index_unchecked(frame.v))
                    {
                        ::uwvm2::utils::container::vector<::std::size_t> comp{};
                        for(;;)
                        {
                            auto const w{st.back()};
                            st.pop_back_unchecked();
                            // Safe: w is guaranteed to be < graph.size() because it came from st
                            on_stack.index_unchecked(w) = false;
                            comp.push_back(w);
                            if(w == frame.v) { break; }
                        }
                        sccs.push_back(::std::move(comp));
                    }

                    // Update parent's low value if this is not the root call
                    if(call_stack.size() > 1uz) [[likely]]
                    {
                        auto& parent_frame{call_stack.index_unchecked(call_stack.size() - 2uz)};
                        low.index_unchecked(parent_frame.v) = ::std::min(low.index_unchecked(parent_frame.v), low.index_unchecked(frame.v));
                    }

                    // Pop this frame
                    call_stack.pop_back_unchecked();
                }
            }
        }

        // 4) Perform ordered DFS within each SCC, only starting from the minimum id vertex on the cycle, avoiding rotation duplicates
        //    Additionally maintain a 64-bit signature set for safe deduplication (based on xxh3), with extremely low collision probability
        ::uwvm2::utils::container::unordered_flat_set<::std::uint_least64_t> seen_signatures{};
        seen_signatures.reserve(256uz);  // Reserve space for cycle signatures to avoid reallocations

        // Temporary buffer for signatures (avoid repeated allocation)
        ::uwvm2::utils::container::vector<::std::size_t> temp_cycle_ids{};
        temp_cycle_ids.reserve(256uz);

        // DFS (restricted to only visit points >= start_id, ensuring minimum id is start_id)
        // Use bool for boolean flags to save memory and improve cache locality
        ::uwvm2::utils::container::vector<bool> in_scc(graph_size, false);
        ::uwvm2::utils::container::vector<bool> on_path(graph_size, false);
        ::uwvm2::utils::container::vector<::std::size_t> path{};
        path.reserve(graph_size);

        // Iterative DFS implementation with recursion depth limit
        struct DfsStackFrame
        {
            ::std::size_t v;
            ::std::size_t start_id;
            ::std::size_t neighbor_idx;
            ::std::size_t depth;
            bool is_initial_call;

            inline constexpr DfsStackFrame(::std::size_t vertex, ::std::size_t start, ::std::size_t current_depth, bool initial = true) noexcept :
                v{vertex}, start_id{start}, neighbor_idx{0uz}, depth{current_depth}, is_initial_call{initial}
            {
            }
        };

        for(auto& comp: sccs)
        {
            // Skip single-node SCCs without cycles (no self-loops)
            if(comp.size() == 1uz)
            {
                auto const u{comp.front_unchecked()};
                bool self_loop{};
                // Safe: u is guaranteed to be < graph.size() because it comes from our graph
                for(auto const w: graph.index_unchecked(u))
                {
                    if(w == u)
                    {
                        self_loop = true;
                        break;
                    }
                }
                if(self_loop)
                {
                    // A -> A
                    temp_cycle_ids.clear();
                    temp_cycle_ids.push_back_unchecked(u);
                    temp_cycle_ids.push_back_unchecked(u);

                    auto const bytes_ptr{reinterpret_cast<::std::byte const*>(temp_cycle_ids.data())};
                    auto const bytes_len{temp_cycle_ids.size() * sizeof(::std::size_t)};
                    auto const sig{::uwvm2::utils::hash::xxh3_64bits(bytes_ptr, bytes_len)};
                    if(!seen_signatures.contains(sig))
                    {
                        seen_signatures.insert(sig);
                        cycle_t cyc{};
                        cyc.reserve(2uz);
                        // Safe: u is guaranteed to be < id_to_name.size() because it comes from our graph
                        cyc.push_back_unchecked(id_to_name.index_unchecked(u));
                        cyc.push_back_unchecked(id_to_name.index_unchecked(u));
                        // Unknown size Use version with checks
                        all_cycles.push_back(::std::move(cyc));
                    }
                }
                continue;
            }

            // Mark nodes in this SCC
            for(auto const v: comp)
            {
                // Safe: v is guaranteed to be < graph.size() because it comes from our graph
                in_scc.index_unchecked(v) = true;
            }

            // Sort by id ascending to ensure cycles starting from minimum id vertex are only enumerated once
            // (comp order is Tarjan's pop order, not guaranteed to be sorted, no forced sorting here to save overhead,
            //  only sort when stability is needed:
            //  std::sort(comp.begin(), comp.end());)

            // If stronger deduplication stability is needed, uncomment the following sort:
            // ::std::sort(comp.begin(), comp.end());

            for(auto const start_id: comp)
            {
                // Clear path state
                for(auto const v: comp)
                {
                    // Safe: v is guaranteed to be < graph.size() because it comes from our graph
                    on_path.index_unchecked(v) = false;
                }
                path.clear();

                auto start_v{start_id};
                // Use explicit stack to avoid recursion with depth limit
                ::uwvm2::utils::container::vector<DfsStackFrame> dfs_stack{};
                dfs_stack.reserve(graph_size);  // Reserve maximum possible depth

                dfs_stack.emplace_back_unchecked(start_v, start_id, 0uz, true);

                while(!dfs_stack.empty()) [[likely]]
                {
                    auto& frame{dfs_stack.back_unchecked()};

                    // Check recursion depth limit
                    if(frame.depth > curr_recursion_depth_limit && curr_recursion_depth_limit != 0uz) [[unlikely]]
                    {
                        // Output warning about recursion depth limit exceeded
                        ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                            u8"uwvm: ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                            u8"[warn]  ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"DFS recursion depth limit \"",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                            curr_recursion_depth_limit,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"\" exceeded at vertex \"",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                            frame.v,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"\". Skipping further checks. Use \"",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                            u8"--wasm-depend-recursion-limit",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"\" to force check. ",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                            u8"(depend)\n",
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                        if(::uwvm2::uwvm::io::depend_warning_fatal) [[unlikely]]
                        {
                            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                u8"uwvm: ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                                u8"[fatal] ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                u8"Convert warnings to fatal errors. ",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                                u8"(depend)\n\n",
                                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                            ::fast_io::fast_terminate();
                        }

                        // Pop this frame and continue with next
                        dfs_stack.pop_back_unchecked();
                        break;
                    }

                    if(frame.is_initial_call)
                    {
                        // Initial call - set up the vertex
                        on_path.index_unchecked(frame.v) = true;
                        path.push_back_unchecked(frame.v);
                        frame.is_initial_call = false;
                    }

                    // Process neighbors
                    auto const& neighbors{graph.index_unchecked(frame.v)};
                    if(frame.neighbor_idx < neighbors.size()) [[likely]]
                    {
                        auto const w{neighbors.index_unchecked(frame.neighbor_idx)};
                        ++frame.neighbor_idx;

                        // Safe: w is guaranteed to be < graph.size() because it comes from graph[v]
                        if(!in_scc.index_unchecked(w)) { continue; }
                        if(w < frame.start_id) { continue; }
                        if(w == frame.start_id)
                        {
                            ::uwvm2::utils::container::vector<::std::size_t> const& path_ids{path};
                            // Normalize to cycle representation starting with minimum id (i.e., start_id): start, ..., start
                            temp_cycle_ids.clear();
                            // size_t operations never wrap around, because the maximum size is signed size_t max.
                            temp_cycle_ids.reserve(path_ids.size() + 1uz);
                            for(auto const id: path_ids) { temp_cycle_ids.push_back_unchecked(id); }
                            temp_cycle_ids.push_back_unchecked(frame.start_id);

                            // Calculate hash signature
                            auto const bytes_ptr{reinterpret_cast<::std::byte const*>(temp_cycle_ids.data())};
                            auto const temp_cycle_ids_size{temp_cycle_ids.size()};

                            // Never overflow, the vector container ensures that the storage size of its contents is always less than the maximum storage
                            // capacity.
                            constexpr auto max_size_dev_size_t{::std::numeric_limits<std::size_t>::max() / sizeof(::std::size_t)};
                            [[assume(temp_cycle_ids_size <= max_size_dev_size_t)]];

                            auto const bytes_len{temp_cycle_ids_size * sizeof(::std::size_t)};
                            auto const sig{::uwvm2::utils::hash::xxh3_64bits(bytes_ptr, bytes_len)};

                            if(seen_signatures.contains(sig)) { continue; }
                            seen_signatures.insert(sig);

                            // Convert back to names and write to result
                            cycle_t cyc{};
                            cyc.reserve(temp_cycle_ids_size);
                            for(auto const id: temp_cycle_ids)
                            {
                                // Safe: id is guaranteed to be < id_to_name.size() because it comes from our graph
                                cyc.push_back_unchecked(id_to_name.index_unchecked(id));
                            }
                            all_cycles.push_back(::std::move(cyc));
                        }
                        else if(!on_path.index_unchecked(w))
                        {
                            // Recursive call - push new frame with increased depth
                            // size_t operations never wrap around, because the maximum size is signed size_t max.
                            dfs_stack.emplace_back_unchecked(w, frame.start_id, frame.depth + 1uz, true);
                            continue;  // Process the new frame first
                        }
                    }
                    else
                    {
                        // All neighbors processed - cleanup
                        path.pop_back_unchecked();
                        on_path.index_unchecked(frame.v) = false;

                        // Pop this frame
                        dfs_stack.pop_back_unchecked();
                    }
                }
            }

            // Clear marks
            for(auto const v: comp)
            {
                // Safe: v is guaranteed to be < graph.size() because it comes from our graph
                in_scc.index_unchecked(v) = false;
            }
        }

        return all_cycles;
    }

    struct build_dependency_graph_and_check_import_exist_ret_t
    {
        using module_name_t = ::uwvm2::utils::container::u8string_view;
        using adjacency_list_t = ::uwvm2::utils::container::unordered_flat_map<module_name_t, ::uwvm2::utils::container::vector<module_name_t>>;

        int ok;
        adjacency_list_t adj;
    };

    /// @brief Build module dependency graph
    /// @return Adjacency list representation of the dependency graph
    inline constexpr build_dependency_graph_and_check_import_exist_ret_t build_dependency_graph_and_check_import_exist() noexcept
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer build_dependency_graph_and_check_import_exist_timer{u8"build dependency graph and check import exist"};
#endif

        using module_name_t = ::uwvm2::utils::container::u8string_view;
        using import_export_name_t = ::uwvm2::utils::container::u8string_view;
        using adjacency_list_t = ::uwvm2::utils::container::unordered_flat_map<module_name_t, ::uwvm2::utils::container::vector<module_name_t>>;

        adjacency_list_t adjacency_list{};
        adjacency_list.reserve(::uwvm2::uwvm::wasm::storage::all_module.size());  // Reserve space for all modules

        // Initialize all module nodes
        for(auto const& module: ::uwvm2::uwvm::wasm::storage::all_module)
        {
            adjacency_list.emplace(module.first, ::uwvm2::utils::container::vector<module_name_t>{});
        }

        // Used to record the contents exported by each module.
        ::uwvm2::utils::container::unordered_flat_map<
            module_name_t,
            ::uwvm2::utils::container::unordered_flat_map<import_export_name_t, ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>>
            exported{};
        exported.reserve(adjacency_list.size());  // Reserve space for all modules to avoid reallocations

        // Build dependency relationships
        for(auto const& curr_module: ::uwvm2::uwvm::wasm::storage::all_module)
        {
            auto const curr_module_name{curr_module.first};

            switch(curr_module.second.type)
            {
                case ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm: [[fallthrough]];
                case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm:
                {
                    auto const exec_wasm_ptr{curr_module.second.module_storage_ptr.wf};
                    auto const exec_wasm_binfmt_ver{exec_wasm_ptr->binfmt_ver};

                    switch(exec_wasm_binfmt_ver)
                    {
                        case 1u:
                        {
                            auto const& exec_wasm_module_storage{exec_wasm_ptr->wasm_module_storage.wasm_binfmt_ver1_storage};
                            auto get_exec_wasm_module_storage_importsec_from_feature_tuple{
                                [&exec_wasm_module_storage]<::uwvm2::parser::wasm::concepts::wasm_feature... Fs> UWVM_ALWAYS_INLINE(
                                    ::uwvm2::utils::container::tuple<Fs...>) constexpr noexcept
                                {
                                    return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<
                                        ::uwvm2::parser::wasm::standard::wasm1::features::import_section_storage_t<Fs...>>(exec_wasm_module_storage.sections);
                                }};
                            auto const& exec_wasm_module_storage_importsec{
                                get_exec_wasm_module_storage_importsec_from_feature_tuple(::uwvm2::uwvm::wasm::feature::all_features)};

                            for(auto const& imports: exec_wasm_module_storage_importsec.imports)
                            {
                                auto const import_module_name{imports.module_name};
                                auto const import_extern_name{imports.extern_name};
                                auto const import_imports_type{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(imports.imports.type)};

                                // Add dependency edge
                                if(adjacency_list.contains(import_module_name))
                                {
                                    auto& deps{adjacency_list[curr_module_name]};
                                    if(deps.empty())
                                    {
                                        deps.reserve(exec_wasm_module_storage_importsec.imports.size());  // Reserve space for all imports
                                    }
                                    // Safe: we just reserved space for all imports
                                    deps.push_back_unchecked(import_module_name);
                                }

                                // Check dependencies
                                auto const import_module{::uwvm2::uwvm::wasm::storage::all_module.find(import_module_name)};
                                if(import_module == ::uwvm2::uwvm::wasm::storage::all_module.end()) [[unlikely]]
                                {
                                    // Output missing module dependency error
                                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                        u8"uwvm: ",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                        u8"[error] ",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"Missing module dependency: \"",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                        import_module_name,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"\" required by \"",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                        curr_module_name,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"\".\n\n",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                    return {static_cast<int>(::uwvm2::uwvm::run::retval::module_dependency_error), ::std::move(adjacency_list)};
                                }

                                // export -> import
                                auto const& imported_module{*import_module};

                                using exported_iterator = decltype(exported)::iterator;

                                exported_iterator curr_exported;  // No init

                                switch(imported_module.second.type)
                                {
                                    case ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm: [[fallthrough]];
                                    case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm:
                                    {
                                        auto const imported_wasm_ptr{imported_module.second.module_storage_ptr.wf};
                                        auto const imported_wasm_binfmt_ver{imported_wasm_ptr->binfmt_ver};

                                        switch(imported_wasm_binfmt_ver)
                                        {
                                            case 1u:
                                            {
                                                auto const& imported_wasm_module_storage{imported_wasm_ptr->wasm_module_storage.wasm_binfmt_ver1_storage};
                                                auto get_imported_wasm_module_storage_exportsec_from_feature_tuple{
                                                    [&imported_wasm_module_storage]<::uwvm2::parser::wasm::concepts::wasm_feature... Fs> UWVM_ALWAYS_INLINE(
                                                        ::uwvm2::utils::container::tuple<Fs...>) constexpr noexcept
                                                    {
                                                        return ::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<
                                                            ::uwvm2::parser::wasm::standard::wasm1::features::export_section_storage_t<Fs...>>(
                                                            imported_wasm_module_storage.sections);
                                                    }};
                                                auto const& imported_wasm_module_storage_exportsec{
                                                    get_imported_wasm_module_storage_exportsec_from_feature_tuple(::uwvm2::uwvm::wasm::feature::all_features)};

                                                // Check if there is an exported map. If not, build one.
                                                auto [curr_exported_module, inserted]{exported.try_emplace(import_module_name)};
                                                if(inserted) [[unlikely]]
                                                {
                                                    curr_exported_module->second.reserve(
                                                        imported_wasm_module_storage_exportsec.exports.size());  // Reserve space for exports
                                                    for(auto const& exports: imported_wasm_module_storage_exportsec.exports)
                                                    {
                                                        auto const export_name{exports.export_name};
                                                        auto const export_type{
                                                            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(exports.exports.type)};
                                                        curr_exported_module->second.emplace(export_name, export_type);
                                                    }
                                                }

                                                curr_exported = curr_exported_module;

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

                                        break;
                                    }
#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))
                                    case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl:
                                    {
                                        auto const imported_dl_ptr{imported_module.second.module_storage_ptr.wd};

                                        // Check if there is an exported map. If not, build one.
                                        auto [curr_exported_module, inserted]{exported.try_emplace(import_module_name)};
                                        if(inserted) [[unlikely]]
                                        {
                                            auto const dl_func{imported_dl_ptr->wasm_dl_storage.capi_function_vec};
                                            auto const dl_func_ptr{dl_func.function_begin};
                                            auto const dl_func_sz{dl_func.function_size};
                                            curr_exported_module->second.reserve(dl_func_sz);  // Reserve space for dl functions
                                            for(auto dl_func_curr{dl_func_ptr}; dl_func_curr != dl_func_ptr + dl_func_sz; ++dl_func_curr)
                                            {
                                                using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;
                                                auto const dl_func_curr_name{
                                                    ::uwvm2::utils::container::u8string_view{
                                                                                             reinterpret_cast<char8_t_const_may_alias_ptr>(dl_func_curr->func_name_ptr),
                                                                                             dl_func_curr->func_name_length}
                                                };
                                                constexpr auto dl_func_type{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(0u)};  // func
                                                curr_exported_module->second.emplace(dl_func_curr_name, dl_func_type);
                                            }
                                        }

                                        curr_exported = curr_exported_module;

                                        break;
                                    }
#endif
                                    [[unlikely]] default:
                                    {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                                        ::std::unreachable();
                                    }
                                }

                                auto const curr_exported_module_exported{curr_exported->second.find(import_extern_name)};
                                if(curr_exported_module_exported == curr_exported->second.end()) [[unlikely]]
                                {
                                    // Output missing export error in module a, dependency from module b
                                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                                        u8"uwvm: ",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                                        u8"[error] ",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"Missing export \"",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                        import_extern_name,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"\" in module \"",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                        import_module_name,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"\" required by \"",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                                        curr_module_name,
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                        u8"\".\n\n",
                                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                    return {static_cast<int>(::uwvm2::uwvm::run::retval::module_dependency_error), ::std::move(adjacency_list)};
                                }

                                constexpr auto get_exceeding_imported_type_name{[]<::std::integral char_type2>(::std::uint_least8_t type) constexpr noexcept
                                                                                    -> ::uwvm2::utils::container::basic_string_view<char_type2>
                                                                                {
                                                                                    switch(type)
                                                                                    {
                                                                                        case 0u:
                                                                                        {
                                                                                            if constexpr(::std::same_as<char_type2, char>) { return {"func"}; }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"func"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"func"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"func"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"func"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        case 1u:
                                                                                        {
                                                                                            if constexpr(::std::same_as<char_type2, char>) { return {"table"}; }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"table"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"table"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"table"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"table"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        case 2u:
                                                                                        {
                                                                                            if constexpr(::std::same_as<char_type2, char>) { return {"mem"}; }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"mem"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"mem"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"mem"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"mem"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        case 3u:
                                                                                        {
                                                                                            if constexpr(::std::same_as<char_type2, char>)
                                                                                            {
                                                                                                return {"global"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"global"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"global"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"global"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"global"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        case 4u:
                                                                                        {
                                                                                            if constexpr(::std::same_as<char_type2, char>) { return {"tag"}; }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"tag"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"tag"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"tag"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"tag"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                        [[unlikely]] default:
                                                                                        {
                                            /// @todo Maybe I forgot to realize it.
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                                                                            ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif

                                                                                            if constexpr(::std::same_as<char_type2, char>)
                                                                                            {
                                                                                                return {"unknown"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, wchar_t>)
                                                                                            {
                                                                                                return {L"unknown"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char8_t>)
                                                                                            {
                                                                                                return {u8"unknown"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char16_t>)
                                                                                            {
                                                                                                return {u"unknown"};
                                                                                            }
                                                                                            else if constexpr(::std::same_as<char_type2, char32_t>)
                                                                                            {
                                                                                                return {U"unknown"};
                                                                                            }
                                                                                            break;
                                                                                        }
                                                                                    }
                                                                                }};

                                auto const curr_exported_module_exported_type{curr_exported_module_exported->second};
                                if(curr_exported_module_exported_type != import_imports_type) [[unlikely]]
                                {
                                    // Output type mismatch error between import and export
                                    ::fast_io::io::perr(
                                        ::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                        u8"[error] ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Type mismatch: import \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        import_extern_name,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" in module \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        curr_module_name,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" expects type ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        get_exceeding_imported_type_name.template operator()<char8_t>(static_cast<::std::uint_least8_t>(import_imports_type)),
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8" but \"",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        import_module_name,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"\" exports type ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        get_exceeding_imported_type_name.template operator()<char8_t>(
                                            static_cast<::std::uint_least8_t>(curr_exported_module_exported_type)),
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8".\n\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));

                                    return {static_cast<int>(::uwvm2::uwvm::run::retval::module_dependency_error), ::std::move(adjacency_list)};
                                }
                            }
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
                    break;
                }
#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))
                case ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl:
                {
                    // Since dl only allows importing functions, there is no possibility of cyclic dependencies or similar issues, so no check is needed
                    break;
                }
#endif
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        return {static_cast<int>(::uwvm2::uwvm::run::retval::ok), ::std::move(adjacency_list)};
    }

    /// @brief Check for duplicates or mismatched imports and exports
    inline int load_and_check_modules() noexcept
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"load and check modules"};
#endif
        // preload abi
        {
            if(::uwvm2::uwvm::wasm::storage::local_preload_wasip1)
            {
                /// @todo
            }

            if(::uwvm2::uwvm::wasm::storage::local_preload_wasip2)
            {
                /// @todo
            }

            if(::uwvm2::uwvm::wasm::storage::local_preload_wasix)
            {
                /// @todo
            }
        }

        // preloaded wasm
        for(auto const& lwc: ::uwvm2::uwvm::wasm::storage::preloaded_wasm)
        {
            if(!::uwvm2::uwvm::wasm::storage::all_module
                    .try_emplace(lwc.module_name,
                                 ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(lwc)},
                                                                            .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm})
                    .second) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    lwc.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }
        }

#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))

        // preloaded dl
        for(auto const& ldc: ::uwvm2::uwvm::wasm::storage::preloaded_dl)
        {
            if(!::uwvm2::uwvm::wasm::storage::all_module
                    .try_emplace(ldc.module_name,
                                 ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wd = ::std::addressof(ldc)},
                                                                            .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl})
                    .second) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    ldc.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }
        }
#endif

        // exec wasm
        {
            if(!::uwvm2::uwvm::wasm::storage::all_module
                    .try_emplace(
                        ::uwvm2::uwvm::wasm::storage::execute_wasm.module_name,
                        ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(::uwvm2::uwvm::wasm::storage::execute_wasm)},
                                                                   .type = ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm})
                    .second) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                    u8"[error] ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Duplicate WASM module names: \"",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                    ::uwvm2::uwvm::wasm::storage::execute_wasm.module_name,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"\".\n\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                return static_cast<int>(::uwvm2::uwvm::run::retval::duplicate_module_name);
            }
        }

        // Build dependency graph and detect cyclic dependencies
        // verbose
        if(::uwvm2::uwvm::io::show_verbose) [[unlikely]]
        {
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_GREEN),
                                u8"[info]  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Start checking whether the import exists and the type matches. ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                u8"(verbose)\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }

        auto const [ok, dependency_graph]{build_dependency_graph_and_check_import_exist()};

        if(ok != static_cast<int>(::uwvm2::uwvm::run::retval::ok)) [[unlikely]] { return ok; }

        // Check Cycle
        if(::uwvm2::uwvm::io::show_depend_warning)
        {
            // verbose
            if(::uwvm2::uwvm::io::show_verbose) [[unlikely]]
            {
                ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                    u8"uwvm: ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_GREEN),
                                    u8"[info]  ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"Start Detecting Cycles. ",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                    u8"(verbose)\n",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
            }

            auto const cycles{detect_cycles(dependency_graph)};

            // Output cyclic dependency warnings
            if(!cycles.empty()) [[unlikely]]
            {
                {
                    // Here, as an entire output, the mutex needs to be controlled uniformly.
                    // There are no unspecified external calls that make the mutex deadlock.

                    // No copies will be made here.
                    auto u8log_output_osr{::fast_io::operations::output_stream_ref(::uwvm2::uwvm::io::u8log_output)};
                    // Add raii locks while unlocking operations
                    ::fast_io::operations::decay::stream_ref_decay_lock_guard u8log_output_lg{
                        ::fast_io::operations::decay::output_stream_mutex_ref_decay(u8log_output_osr)};
                    // No copies will be made here.
                    auto u8log_output_ul{::fast_io::operations::decay::output_stream_unlocked_ref_decay(u8log_output_osr)};

                    // output warning
                    ::fast_io::io::perr(u8log_output_ul,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                        u8"[warn]  ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Detected ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                        cycles.size(),
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8" cyclic dependencies: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                        u8"(depend)\n");

                    ::std::size_t counter{};

                    for(auto const& cycle: cycles)
                    {
                        ::fast_io::io::perr(u8log_output_ul,
                                            ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                            u8"              ",
                                            ++counter,
                                            u8": ");

                        bool is_not_first{};
                        for(auto const cycle_curr: cycle)
                        {
                            if(is_not_first) [[likely]]
                            {
                                ::fast_io::io::perr(u8log_output_ul,
                                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                                    u8" -> ");
                            }
                            is_not_first = true;

                            ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN), cycle_curr);
                        }

                        ::fast_io::io::perrln(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                    }
                }

                if(::uwvm2::uwvm::io::depend_warning_fatal) [[unlikely]]
                {
                    ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                        u8"uwvm: ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_RED),
                                        u8"[fatal] ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                        u8"Convert warnings to fatal errors. ",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                        u8"(depend)\n\n",
                                        ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
                    ::fast_io::fast_terminate();
                }
            }
        }

        return static_cast<int>(::uwvm2::uwvm::run::retval::ok);
    }
}  // namespace uwvm2::uwvm::run

#ifndef UWVM_MODULE
// macro
# include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <uwvm2/utils/macro/pop_macros.h>
#endif
