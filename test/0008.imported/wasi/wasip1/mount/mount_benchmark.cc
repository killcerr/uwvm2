/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      GPT-5
 * @version     2.0.0
 * @copyright   APL-2.0 License
 */

#include <cstddef>
#include <cstdint>

#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <fast_io_hosted/white_hole/white_hole.h>

#include <uwvm2/utils/container/impl.h>
#include <uwvm2/imported/wasi/wasip1/mount_root/mount.h>

using namespace uwvm2::imported::wasi::wasip1::mount_root;
namespace C = ::uwvm2::utils::container;

static void bench_match(std::size_t patterns, std::size_t paths)
{
    fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> rng{};
    auto rand_char = [&]() -> char8_t
    {
        static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
        std::size_t const idx = static_cast<std::size_t>(rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
        return pool[idx];
    };
    auto randsz = [&](std::size_t lo, std::size_t hi) -> std::size_t
    {
        std::size_t const span = (hi - lo + 1);
        return lo + (static_cast<std::size_t>(rng()) % span);
    };

    // Prepare entry with compiled automata
    mount_root_entry entry{};
    C::vector<C::u8string> store{};
    store.reserve(patterns + 1);
    for(std::size_t i{}; i < patterns; ++i)
    {
        std::size_t const n = randsz(4, 24);
        C::u8string pat{};
        pat.reserve(n);
        for(std::size_t j{}; j < n; ++j) { pat.push_back(rand_char()); }
        store.emplace_back(::std::move(pat));
        auto v = C::u8string_view{store.back().data(), store.back().size()};
        auto res = parse_pattern(v);
        entry.add_patterns.emplace_back(v);
        entry.add_automata.emplace_back(build_nfa_from_tokens(res.tokens));
    }

    // Prepare random path set
    C::vector<C::u8string> paths_store{};
    paths_store.reserve(paths);
    for(std::size_t i{}; i < paths; ++i)
    {
        std::size_t const n = randsz(4, 48);
        C::u8string p{};
        p.reserve(n);
        for(std::size_t j{}; j < n; ++j) { p.push_back(rand_char()); }
        paths_store.emplace_back(::std::move(p));
    }

    // Benchmark: pure match
    std::size_t matches{};
    {
        fast_io::timer t(u8"mount_benchmark: match_any over paths");
        for(auto const& p: paths_store)
        {
            auto ok = match_any(C::u8string_view{p.data(), p.size()}, entry.add_automata);
            matches += ok ? 1u : 0u;
        }
    }
    ::fast_io::io::perr(::fast_io::u8err(), u8"matches=", matches, u8"\n");

    // Benchmark: evaluate_path_access
    std::size_t allows{};
    {
        fast_io::timer t(u8"mount_benchmark: evaluate_path_access over paths");
        for(auto const& p: paths_store)
        {
            auto pol = evaluate_path_access(entry, C::u8string_view{p.data(), p.size()}, false, false, false);
            allows += (pol != access_policy::deny) ? 1u : 0u;
        }
    }
    ::fast_io::io::perr(::fast_io::u8err(), u8"allows=", allows, u8"\n");
}

// Benchmark: 10,000 small cases (<5 pattern length, <50 text length)
static void bench_small_patterns_10k()
{
    fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> rng{};
    auto rand_char = [&]() -> char8_t
    {
        static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/.*?{}\\,";
        std::size_t const idx = static_cast<std::size_t>(rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
        return pool[idx];
    };

    std::size_t matches{};
    {
        fast_io::timer t(u8"mount_benchmark: small patterns (10k)");
        for(std::size_t i{}; i < 10000u; ++i)
        {
            std::size_t const pat_len = 1u + static_cast<std::size_t>(rng()) % 4u;   // 1..4
            std::size_t const txt_len = 1u + static_cast<std::size_t>(rng()) % 49u;  // 1..49

            C::u8string pat{};
            pat.reserve(pat_len);
            for(std::size_t j{}; j < pat_len; ++j) { pat.push_back(rand_char()); }
            auto const pat_view = C::u8string_view{pat.data(), pat.size()};
            auto const parsed = parse_pattern(pat_view);
            auto const automaton = build_nfa_from_tokens(parsed.tokens);

            C::u8string txt{};
            txt.reserve(txt_len);
            for(std::size_t j{}; j < txt_len; ++j) { txt.push_back(rand_char()); }
            auto const txt_view = C::u8string_view{txt.data(), txt.size()};

            bool const ok = match_compiled_pattern(txt_view, automaton);
            matches += ok ? 1u : 0u;
        }
    }
    ::fast_io::io::perr(::fast_io::u8err(), u8"small_matches=", matches, u8"\n");
}

int main()
{
    ::fast_io::io::perr(::fast_io::u8err(), u8"[RUN] mount benchmark\n");
    bench_match(200, 20000);
    bench_small_patterns_10k();
    ::fast_io::io::perr(::fast_io::u8err(), u8"[OK] benchmark done\n");
    return 0;
}

