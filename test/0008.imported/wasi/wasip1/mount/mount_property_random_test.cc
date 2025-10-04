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
#include <fast_io_driver/timer.h>
#include <fast_io_hosted/white_hole/white_hole.h>

#include <uwvm2/utils/container/impl.h>
#include <uwvm2/imported/wasi/wasip1/mount_root/mount.h>

using namespace uwvm2::imported::wasi::wasip1::mount_root;
namespace C = ::uwvm2::utils::container;

static void require(bool cond, char const* msg)
{
	if(!cond)
	{
		std::fprintf(stderr, "[FAIL] %s\n", msg);
		std::fflush(stderr);
		::fast_io::fast_terminate();
	}
}

static char8_t rand_alpha(fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> &rng)
{
	static constexpr char8_t letters[] = u8"abcdefghijklmnopqrstuvwxyz";
	std::size_t const idx = static_cast<std::size_t>(rng()) % (sizeof(letters) / sizeof(letters[0]) - 1);
	return letters[idx];
}

static C::u8string gen_literal(fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> &rng, std::size_t lo, std::size_t hi)
{
	std::size_t span = hi - lo + 1;
	std::size_t n = lo + static_cast<std::size_t>(rng()) % span;
	C::u8string s{}; s.reserve(n);
	for(std::size_t i{}; i < n; ++i) { s.push_back(rand_alpha(rng)); }
	return s;
}

static C::u8string gen_safe_pattern(fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> &rng)
{
	// Build slash-separated segments; allow segments of literal, *, ?, or alternatives; occasionally insert "**" as a segment
	std::size_t const segs = 1u + static_cast<std::size_t>(rng()) % 5u; // 1..5 segments
	C::vector<C::u8string> parts{}; parts.reserve(segs);
	for(std::size_t i{}; i < segs; ++i)
	{
		std::size_t const pick = static_cast<std::size_t>(rng()) % 10u;
		if(pick <= 4u)
		{
			parts.emplace_back(gen_literal(rng, 1, 6));
		}
		else if(pick <= 6u)
		{
			parts.emplace_back(C::u8string{u8"*"});
		}
		else if(pick == 7u)
		{
			parts.emplace_back(C::u8string{u8"?"});
		}
		else if(pick == 8u)
		{
			// alternatives like {a,b}
			C::u8string a = gen_literal(rng, 1, 2);
			C::u8string b = gen_literal(rng, 1, 2);
			C::u8string alt{};
			alt.reserve(4 + a.size() + b.size());
			alt.push_back(u8'{'); alt.append(a); alt.push_back(u8','); alt.append(b); alt.push_back(u8'}');
			parts.emplace_back(::std::move(alt));
		}
		else
		{
			// "**" only as full segment is safe
			parts.emplace_back(C::u8string{u8"**"});
		}
	}
	// Join with '/'
	C::u8string pat{};
	std::size_t total{};
	for(auto const &p: parts) { total += p.size(); }
	pat.reserve(total + (segs ? segs - 1 : 0));
	for(std::size_t i{}; i < parts.size(); ++i)
	{
		if(i) pat.push_back(u8'/');
		pat.append(parts[i]);
	}
	// maybe trailing slash
	if((rng() & 7u) == 0u) { pat.push_back(u8'/'); }
	return pat;
}

static C::u8string gen_rel_path(fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> &rng)
{
	// Path of 1..6 segments with literal letters; no wildcards
	std::size_t const segs = 1u + static_cast<std::size_t>(rng()) % 6u;
	C::u8string path{};
	for(std::size_t i{}; i < segs; ++i)
	{
		if(i) path.push_back(u8'/');
		std::size_t const n = 1u + static_cast<std::size_t>(rng()) % 6u;
		for(std::size_t j{}; j < n; ++j) { path.push_back(rand_alpha(rng)); }
	}
	return path;
}

static void run_property_cases(std::size_t cases)
{
	fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> rng{};

	for(std::size_t it{}; it < cases; ++it)
	{
		mount_root_entry entry{};
		// Random counts: 0..14, encourage >10 sometimes
		std::size_t const addn = static_cast<std::size_t>(rng()) % 15u;
		std::size_t const rmn  = static_cast<std::size_t>(rng()) % 15u;
		std::size_t const escn = static_cast<std::size_t>(rng()) % 15u;

		// Generate patterns and automata
		C::vector<C::u8string> backing{}; backing.reserve(addn + rmn + escn + 8u);
		auto add_pat = [&](std::size_t n, C::vector<compiled_pattern_automaton> &out_autos, C::vector<C::u8string_view> &out_raw)
		{
			for(std::size_t i{}; i < n; ++i)
			{
				C::u8string p = gen_safe_pattern(rng);
				backing.emplace_back(::std::move(p));
				C::u8string_view v{backing.back().data(), backing.back().size()};
				auto res = parse_pattern(v);
				require(!res.error.has_error, "parse_pattern error in property test");
				out_raw.emplace_back(v);
				out_autos.emplace_back(build_nfa_from_tokens(res.tokens));
			}
		};
		add_pat(addn, entry.add_automata, entry.add_patterns);
		add_pat(rmn,  entry.rm_automata,  entry.rm_patterns);
		add_pat(escn, entry.symlink_escape_automata, entry.symlink_escape_patterns);

		// Randomly seed created_by_wasi_set with 0..20 created paths
		C::vector<C::u8string> created_store{}; created_store.reserve(20u);
		std::size_t const createdn = static_cast<std::size_t>(rng()) % 21u;
		for(std::size_t i{}; i < createdn; ++i)
		{
			C::u8string cp = gen_rel_path(rng);
			created_store.emplace_back(::std::move(cp));
			C::u8string_view v{created_store.back().data(), created_store.back().size()};
			wasi_mark_created(entry, v);
		}

		// Random test path
		C::u8string path = gen_rel_path(rng);
		C::u8string_view rel_view{path.data(), path.size()};

		// Random flags: at most one of (is_symlink, is_symlink_creation)
		bool is_symlink = (rng() & 7u) == 0u;
		bool is_symlink_creation = (!is_symlink) && ((rng() & 7u) == 0u);
		bool is_wasi_created = (rng() & 3u) == 0u; // 1/4 chance

		// Random symlink target: sometimes empty, sometimes a created path
		C::u8string symlink_target{};
		C::u8string_view symlink_target_view{};
		if((rng() & 3u) == 0u && !created_store.empty())
		{
			std::size_t idx = static_cast<std::size_t>(rng()) % created_store.size();
			symlink_target_view = C::u8string_view{created_store[idx].data(), created_store[idx].size()};
		}

		// Actual policy
		auto pol = evaluate_path_access(entry, rel_view, is_symlink, is_wasi_created, is_symlink_creation, symlink_target_view);

		// Compute expected policy according to the documented precedence
		auto matches_any = [&](C::vector<compiled_pattern_automaton> const& autos) -> bool { return match_any(rel_view, autos); };
		bool hit_escape = matches_any(entry.symlink_escape_automata);
		uwvm2::imported::wasi::wasip1::mount_root::access_policy expected = access_policy::allow;
		if(is_symlink && hit_escape)
		{
			expected = access_policy::allow_bypass_root;
		}
		else if(is_symlink_creation && hit_escape)
		{
			expected = access_policy::deny;
		}
		else if(!symlink_target_view.empty())
		{
			if(entry.created_by_wasi_set.contains(symlink_target_view)) { expected = access_policy::allow; }
		}
		else if(is_wasi_created)
		{
			expected = access_policy::allow;
		}
		else if(!entry.add_automata.empty() && matches_any(entry.add_automata))
		{
			expected = access_policy::allow;
		}
		else if(!entry.rm_automata.empty() && matches_any(entry.rm_automata))
		{
			expected = access_policy::deny;
		}
		else
		{
			expected = access_policy::allow;
		}

		require(pol == expected, "evaluate_path_access precedence mismatch");
		bool allow_bool = wasi_rule_allow_open(entry, rel_view, is_symlink, is_wasi_created, is_symlink_creation, nullptr, symlink_target_view);
		require(allow_bool == (pol != access_policy::deny), "wasi_rule_allow_open result inconsistent with policy");
	}
}

int main()
{
	std::fputs("[RUN] mount_property_random_test: 1000 randomized cases\n", stderr);
	run_property_cases(1000);
	std::fputs("[OK] property tests passed\n", stderr);
	return 0;
}


