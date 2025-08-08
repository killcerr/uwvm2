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
        using module_name_t = ::uwvm2::utils::container::u8string_view;
        using cycle_t = ::uwvm2::utils::container::vector<module_name_t>;
        using cycles_t = ::uwvm2::utils::container::vector<cycle_t>;

        cycles_t all_cycles{};
        if(adjacency_list.empty()) [[unlikely]] { return all_cycles; }

        // 1) Compress names to integer ids for efficient computation (avoid frequent hashing of string_view)
        ::uwvm2::utils::container::unordered_flat_map<module_name_t, ::std::uint32_t> name_to_id{};
        ::uwvm2::utils::container::vector<module_name_t> id_to_name{};
        id_to_name.reserve(adjacency_list.size());
        name_to_id.reserve(adjacency_list.size());
        for(auto const& [name, _] : adjacency_list)
        {
            auto const id = static_cast<::std::uint32_t>(id_to_name.size());
            id_to_name.push_back(name);
            name_to_id.emplace(name, id);
        }

        // 2) Build compact adjacency list (by id)
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::vector<::std::uint32_t>> graph{};
        graph.resize(id_to_name.size());
        for(auto const& [u_name, outs] : adjacency_list)
        {
            auto const u_id = name_to_id[u_name];
            auto& u_out = graph[u_id];
            u_out.reserve(outs.size());
            for(auto const v_name : outs)
            {
                auto const it = name_to_id.find(v_name);
                if(it != name_to_id.end()) { u_out.push_back(it->second); }
            }
        }

        // 3) Tarjan's algorithm to find strongly connected components (O(V+E))
        ::uwvm2::utils::container::vector<int> index(graph.size(), -1);
        ::uwvm2::utils::container::vector<int> low(graph.size(), 0);
        ::uwvm2::utils::container::vector<::std::uint8_t> on_stack(graph.size(), 0);
        ::uwvm2::utils::container::vector<::std::uint32_t> st{};
        st.reserve(graph.size());
        int dfs_idx{};
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::vector<::std::uint32_t>> sccs{};

        auto strongconnect = [&](this auto const& self, ::std::uint32_t v) noexcept -> void
        {
            index[v] = low[v] = dfs_idx++;
            st.push_back(v);
            on_stack[v] = 1;

            for(auto const w : graph[v])
            {
                if(index[w] == -1)
                {
                    self(w);
                    low[v] = (low[v] < low[w] ? low[v] : low[w]);
                }
                else if(on_stack[w])
                {
                    low[v] = (low[v] < index[w] ? low[v] : index[w]);
                }
            }

            if(low[v] == index[v])
            {
                ::uwvm2::utils::container::vector<::std::uint32_t> comp{};
                for(;;)
                {
                    auto const w = st.back();
                    st.pop_back_unchecked();
                    on_stack[w] = 0;
                    comp.push_back(w);
                    if(w == v) { break; }
                }
                sccs.push_back(::std::move(comp));
            }
        };

        for(::std::uint32_t v{}; v < graph.size(); ++v)
        {
            if(index[v] == -1) { strongconnect(v); }
        }

        // 4) Perform ordered DFS within each SCC, only starting from the minimum id vertex on the cycle, avoiding rotation duplicates
        //    Additionally maintain a 64-bit signature set for safe deduplication (based on xxh3), with extremely low collision probability
        ::uwvm2::utils::container::unordered_flat_set<::std::uint64_t> seen_signatures{};

        // Temporary buffer for signatures (avoid repeated allocation)
        ::uwvm2::utils::container::vector<::std::uint32_t> temp_cycle_ids{};
        temp_cycle_ids.reserve(256);

        auto emit_cycle = [&](::uwvm2::utils::container::vector<::std::uint32_t> const& path_ids,
                              ::std::uint32_t start_id) noexcept -> void
        {
            // Normalize to cycle representation starting with minimum id (i.e., start_id): start, ..., start
            temp_cycle_ids.clear();
            temp_cycle_ids.reserve(path_ids.size() + 1);
            for(auto const id : path_ids) { temp_cycle_ids.push_back(id); }
            temp_cycle_ids.push_back(start_id);

            // Calculate hash signature
            auto const bytes_ptr = reinterpret_cast<::std::byte const*>(temp_cycle_ids.data());
            auto const bytes_len = temp_cycle_ids.size() * sizeof(::std::uint32_t);
            auto const sig = ::uwvm2::utils::hash::xxh3_64bits(bytes_ptr, bytes_len);
            if(seen_signatures.contains(sig)) { return; }
            seen_signatures.insert(sig);

            // Convert back to names and write to result
            cycle_t cyc{};
            cyc.reserve(temp_cycle_ids.size());
            for(auto const id : temp_cycle_ids) { cyc.push_back(id_to_name[id]); }
            all_cycles.push_back(::std::move(cyc));
        };

        // DFS (restricted to only visit points >= start_id, ensuring minimum id is start_id)
        ::uwvm2::utils::container::vector<::std::uint8_t> in_scc(graph.size(), 0);
        ::uwvm2::utils::container::vector<::std::uint8_t> on_path(graph.size(), 0);
        ::uwvm2::utils::container::vector<::std::uint32_t> path{};
        path.reserve(graph.size());

        auto dfs_cycle = [&](this auto const& self, ::std::uint32_t v, ::std::uint32_t start_id) noexcept -> void
        {
            on_path[v] = 1;
            path.push_back(v);
            for(auto const w : graph[v])
            {
                if(!in_scc[w]) { continue; }
                if(w < start_id) { continue; }
                if(w == start_id)
                {
                    emit_cycle(path, start_id);
                }
                else if(!on_path[w])
                {
                    self(w, start_id);
                }
            }
            path.pop_back_unchecked();
            on_path[v] = 0;
        };

        for(auto& comp : sccs)
        {
            // Skip single-node SCCs without cycles (no self-loops)
            if(comp.size() == 1)
            {
                auto const u = comp.front();
                bool self_loop{};
                for(auto const w : graph[u])
                {
                    if(w == u) { self_loop = true; break; }
                }
                if(self_loop)
                {
                    // A -> A
                    temp_cycle_ids.clear();
                    temp_cycle_ids.push_back(u);
                    temp_cycle_ids.push_back(u);

                    auto const bytes_ptr = reinterpret_cast<::std::byte const*>(temp_cycle_ids.data());
                    auto const bytes_len = temp_cycle_ids.size() * sizeof(::std::uint32_t);
                    auto const sig = ::uwvm2::utils::hash::xxh3_64bits(bytes_ptr, bytes_len);
                    if(!seen_signatures.contains(sig))
                    {
                        seen_signatures.insert(sig);
                        cycle_t cyc{};
                        cyc.reserve(2);
                        cyc.push_back(id_to_name[u]);
                        cyc.push_back(id_to_name[u]);
                        all_cycles.push_back(::std::move(cyc));
                    }
                }
                continue;
            }

            // Mark nodes in this SCC
            for(auto const v : comp) { in_scc[v] = 1; }

            // Sort by id ascending to ensure cycles starting from minimum id vertex are only enumerated once
            // (comp order is Tarjan's pop order, not guaranteed to be sorted, no forced sorting here to save overhead,
            //  only sort when stability is needed:
            //  std::sort(comp.begin(), comp.end());)

            // If stronger deduplication stability is needed, uncomment the following sort:
            // ::std::sort(comp.begin(), comp.end());

            for(auto const start_id : comp)
            {
                // Clear path state
                for(auto const v : comp) { on_path[v] = 0; }
                path.clear();

                dfs_cycle(start_id, start_id);
            }

            // Clear marks
            for(auto const v : comp) { in_scc[v] = 0; }
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
        using module_name_t = ::uwvm2::utils::container::u8string_view;
        using import_export_name_t = ::uwvm2::utils::container::u8string_view;
        using adjacency_list_t = ::uwvm2::utils::container::unordered_flat_map<module_name_t, ::uwvm2::utils::container::vector<module_name_t>>;

        adjacency_list_t adjacency_list{};

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
                                if(adjacency_list.contains(import_module_name)) { adjacency_list[curr_module_name].push_back(import_module_name); }

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
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(lwc.module_name)) [[unlikely]]
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

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                lwc.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(lwc)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_wasm});
        }

#if (defined(_WIN32) || defined(__CYGWIN__)) && (!defined(__CYGWIN__) && !defined(__WINE__)) ||                                                                \
    ((!defined(_WIN32) || defined(__WINE__)) && (__has_include(<dlfcn.h>) && (defined(__CYGWIN__) || (!defined(__NEWLIB__) && !defined(__wasi__)))))

        // preloaded dl
        for(auto const& ldc: ::uwvm2::uwvm::wasm::storage::preloaded_dl)
        {
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(ldc.module_name)) [[unlikely]]
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

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                ldc.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wd = ::std::addressof(ldc)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::preloaded_dl});
        }
#endif

        // exec wasm
        {
            if(::uwvm2::uwvm::wasm::storage::all_module.contains(::uwvm2::uwvm::wasm::storage::execute_wasm.module_name)) [[unlikely]]
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

            ::uwvm2::uwvm::wasm::storage::all_module.emplace(
                ::uwvm2::uwvm::wasm::storage::execute_wasm.module_name,
                ::uwvm2::uwvm::wasm::storage::all_module_t{.module_storage_ptr = {.wf = ::std::addressof(::uwvm2::uwvm::wasm::storage::execute_wasm)},
                                                           .type = ::uwvm2::uwvm::wasm::storage::module_type_t::exec_wasm});
        }

        // Build dependency graph and detect cyclic dependencies
        auto const [ok, dependency_graph]{build_dependency_graph_and_check_import_exist()};

        if(ok != static_cast<int>(::uwvm2::uwvm::run::retval::ok)) [[unlikely]] { return ok; }

        // Check Cycle
        if(::uwvm2::uwvm::io::show_depend_warning)
        {
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
