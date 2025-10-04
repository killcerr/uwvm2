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
#include <cstdio>
#include <cstdlib>
#include <random>
#include <initializer_list>

// Bring container aliases (u8string, u8string_view, vector, ...)
#include <uwvm2/utils/container/impl.h>
// System-entropy RNG (fast_io white_hole)
#include <fast_io_hosted/white_hole/white_hole.h>

// Include the implementation under test to access internal helpers
#include <uwvm2/imported/wasi/wasip1/mount_root/mount.h>

using namespace uwvm2::imported::wasi::wasip1::mount_root;
namespace C = ::uwvm2::utils::container;

static void require(bool cond, char const* msg)
{
	if(!cond)
	{
		std::fprintf(stderr, "[FAIL] %s\n", msg);
		std::fflush(stderr);
		std::abort();
	}
}

static void test_parse_valid_invalid()
{
	// Valid patterns should have no error
	for(auto const pat: {u8"*", u8"**", u8"?", u8"abc",
	                    u8"data/{a,b}/**/*.wasm", u8"a\\*b", u8"{x,y,z}", u8"**/foo"})
	{
		auto res = ::uwvm2::imported::wasi::wasip1::mount_root::parse_pattern(uwvm2::utils::container::u8string_view{::fast_io::mnp::os_c_str(pat)});
		require(!res.error.has_error, "parse_pattern valid case unexpectedly errored");
	}

	// Invalid patterns should report error
	for(auto const pat: {u8"a**b", u8"{", u8"}", u8"{}", u8"\\"})
	{
		auto res = ::uwvm2::imported::wasi::wasip1::mount_root::parse_pattern(uwvm2::utils::container::u8string_view{::fast_io::mnp::os_c_str(pat)});
		require(res.error.has_error, "parse_pattern invalid case did not error");
	}
}

static bool matches(uwvm2::utils::container::u8string_view pat, uwvm2::utils::container::u8string_view path)
{
	auto res = ::uwvm2::imported::wasi::wasip1::mount_root::parse_pattern(pat);
	if(res.error.has_error) { return false; }
	auto aut = ::uwvm2::imported::wasi::wasip1::mount_root::build_nfa_from_tokens(res.tokens);
	return ::uwvm2::imported::wasi::wasip1::mount_root::match_nfa(aut, path);
}

static void test_nfa_match_basic()
{
	// literal
	require(matches(uwvm2::utils::container::u8string_view{u8"abc"}, uwvm2::utils::container::u8string_view{u8"abc"}), "literal match failed");
	require(!matches(uwvm2::utils::container::u8string_view{u8"abc"}, uwvm2::utils::container::u8string_view{u8"ab"}), "literal negative failed");

	// ? (non-slash)
	require(matches(uwvm2::utils::container::u8string_view{u8"a?c"}, uwvm2::utils::container::u8string_view{u8"abc"}), "? basic failed");
	require(!matches(uwvm2::utils::container::u8string_view{u8"a?c"}, uwvm2::utils::container::u8string_view{u8"a/c"}), "? slash should not match");

	// * (zero or more non-slash)
	require(matches(uwvm2::utils::container::u8string_view{u8"a*c"}, uwvm2::utils::container::u8string_view{u8"ac"}), "* zero failed");
	require(matches(uwvm2::utils::container::u8string_view{u8"a*c"}, uwvm2::utils::container::u8string_view{u8"abbbc"}), "* multi failed");
	require(!matches(uwvm2::utils::container::u8string_view{u8"a*c"}, uwvm2::utils::container::u8string_view{u8"a/bc"}), "* should not cross slash");

	// ** (can cross slash)
	require(matches(uwvm2::utils::container::u8string_view{u8"**/foo"}, uwvm2::utils::container::u8string_view{u8"a/b/foo"}), "** cross failed");
	require(matches(uwvm2::utils::container::u8string_view{u8"**/foo"}, uwvm2::utils::container::u8string_view{u8"foo"}), "** zero failed");
	// optional trailing slash behavior
	require(matches(uwvm2::utils::container::u8string_view{u8"dir/"}, uwvm2::utils::container::u8string_view{u8"dir"}), "dir/ should match dir");
	require(!matches(uwvm2::utils::container::u8string_view{u8"dir/"}, uwvm2::utils::container::u8string_view{u8"dirx"}), "dir/ should not match dirx");

	// alternatives
	require(matches(uwvm2::utils::container::u8string_view{u8"{a,b}/x"}, uwvm2::utils::container::u8string_view{u8"a/x"}), "alt a failed");
	require(matches(uwvm2::utils::container::u8string_view{u8"{a,b}/x"}, uwvm2::utils::container::u8string_view{u8"b/x"}), "alt b failed");
	require(!matches(uwvm2::utils::container::u8string_view{u8"{a,b}/x"}, uwvm2::utils::container::u8string_view{u8"c/x"}), "alt negative failed");

	// escape
	require(matches(uwvm2::utils::container::u8string_view{u8"a\\*b"}, uwvm2::utils::container::u8string_view{u8"a*b"}), "escape * failed");
}

static void test_access_policy()
{
	mount_root_entry entry{};
	// Prepare whitelist and blacklist
	{
		// add: allow/**
		auto add_res = parse_pattern(uwvm2::utils::container::u8string_view{u8"allow/**"});
		require(!add_res.error.has_error, "parse allow/** failed");
		entry.add_patterns.emplace_back(uwvm2::utils::container::u8string_view{u8"allow/**"});
		entry.add_automata.emplace_back(build_nfa_from_tokens(add_res.tokens));
	}
	{
		// rm: deny/**
		auto rm_res = parse_pattern(uwvm2::utils::container::u8string_view{u8"deny/**"});
		require(!rm_res.error.has_error, "parse deny/** failed");
		entry.rm_patterns.emplace_back(uwvm2::utils::container::u8string_view{u8"deny/**"});
		entry.rm_automata.emplace_back(build_nfa_from_tokens(rm_res.tokens));
	}
	{
		// symlink-escape: link/**
		auto se_res = parse_pattern(uwvm2::utils::container::u8string_view{u8"link/**"});
		require(!se_res.error.has_error, "parse link/** failed");
		entry.symlink_escape_patterns.emplace_back(uwvm2::utils::container::u8string_view{u8"link/**"});
		entry.symlink_escape_automata.emplace_back(build_nfa_from_tokens(se_res.tokens));
	}

	// allow/** → allow
	{
		auto pol = evaluate_path_access(entry, uwvm2::utils::container::u8string_view{u8"allow/x"}, false, false, false);
		require(pol == access_policy::allow, "allow/** policy failed");
	}
	// deny/** → deny
	{
		auto pol = evaluate_path_access(entry, uwvm2::utils::container::u8string_view{u8"deny/x"}, false, false, false);
		require(pol == access_policy::deny, "deny/** policy failed");
	}
	// default → allow
	{
		auto pol = evaluate_path_access(entry, uwvm2::utils::container::u8string_view{u8"other/x"}, false, false, false);
		require(pol == access_policy::allow, "default allow policy failed");
	}
	// symlink escape hit
	{
		auto pol = evaluate_path_access(entry, uwvm2::utils::container::u8string_view{u8"link/abc"}, true, false, false);
		require(pol == access_policy::allow_bypass_root, "symlink escape allow_bypass_root failed");
	}
	// symlink creation hits symlink-escape → deny
	{
		auto pol = evaluate_path_access(entry, uwvm2::utils::container::u8string_view{u8"link/abc"}, false, false, true);
		require(pol == access_policy::deny, "symlink creation should be denied when matching escape");
	}
}

static void test_dir_filter()
{
	mount_root_entry entry{};
	// whitelist: visible/**
	auto add_res = parse_pattern(uwvm2::utils::container::u8string_view{u8"visible/**"});
	require(!add_res.error.has_error, "parse visible/** failed");
	entry.add_patterns.emplace_back(uwvm2::utils::container::u8string_view{u8"visible/**"});
	entry.add_automata.emplace_back(build_nfa_from_tokens(add_res.tokens));

	::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string_view> names{};
	names.emplace_back(::uwvm2::utils::container::u8string_view{u8"visible.txt"});
	names.emplace_back(::uwvm2::utils::container::u8string_view{u8"hidden.txt"});

	::uwvm2::utils::container::vector<bool> is_symlink{};
	is_symlink.assign(2, false);

	::uwvm2::utils::container::vector<bool> out_allowed{};
	wasi_filter_directory_entries(entry, ::uwvm2::utils::container::u8string_view{u8""}, names, is_symlink, out_allowed);
	require(out_allowed.size() == 2, "out_allowed size mismatch");
	require(out_allowed[0] == true, "visible should be allowed");
	require(out_allowed[1] == false, "hidden should be denied by default (no match) if blacklist later present");

}

static void fuzz_parse_and_match(std::size_t iterations)
{
	fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> entropy_rng{};
	auto rand_char = [&]() -> char8_t
	{
		static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
		std::size_t const idx = static_cast<std::size_t>(entropy_rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
		return pool[idx];
	};
	auto randint = [&](int lo, int hi) -> int
	{
		std::size_t const span = static_cast<std::size_t>(hi - lo + 1);
		return lo + static_cast<int>(static_cast<std::size_t>(entropy_rng()) % span);
	};

	for(std::size_t it{}; it < iterations; ++it)
	{
		// build a random small pattern and path
		::uwvm2::utils::container::u8string pat{};
		::uwvm2::utils::container::u8string pth{};
		int n1 = randint(1, 12), n2 = randint(1, 12);
		for(int i = 0; i < n1; ++i) { pat.push_back(rand_char()); }
		for(int i = 0; i < n2; ++i) { pth.push_back(rand_char()); }

		auto res = parse_pattern(uwvm2::utils::container::u8string_view{pat.data(), pat.size()});
		if(!res.error.has_error)
		{
			auto aut = build_nfa_from_tokens(res.tokens);
			(void)match_nfa(aut, uwvm2::utils::container::u8string_view{pth.data(), pth.size()});
		}
	}
}

// Parse fuzz: random patterns, always attempt build; optionally skip match; focus on crash-only
static void fuzz_parse_only(std::size_t iterations)
{
	fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> entropy_rng{};
	auto rand_char = [&]() -> char8_t
	{
		static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
		std::size_t const idx = static_cast<std::size_t>(entropy_rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
		return pool[idx];
	};
	auto randsz = [&](std::size_t lo, std::size_t hi) -> std::size_t
	{
		std::size_t const span = (hi - lo + 1);
		return lo + (static_cast<std::size_t>(entropy_rng()) % span);
	};

	for(std::size_t it{}; it < iterations; ++it)
	{
		// Random pattern length [0, 256]
		C::u8string pat{};
		pat.reserve(256);
		std::size_t const n = randsz(0, 256);
		for(std::size_t i{}; i < n; ++i) { pat.push_back(rand_char()); }

		auto res = parse_pattern(C::u8string_view{pat.data(), pat.size()});
		// Always attempt to build NFA from returned tokens, regardless of error
		(void)build_nfa_from_tokens(res.tokens);
	}
}

// Access policy fuzz: random entry config and random requests; crash-only
static void fuzz_access_policy_random(std::size_t iterations)
{
	fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> entropy_rng{};
	auto rand_char = [&]() -> char8_t
	{
		static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
		std::size_t const idx = static_cast<std::size_t>(entropy_rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
		return pool[idx];
	};
	auto randsz = [&](std::size_t lo, std::size_t hi) -> std::size_t
	{
		std::size_t const span = (hi - lo + 1);
		return lo + (static_cast<std::size_t>(entropy_rng()) % span);
	};
	auto randbool = [&]() -> bool { return (entropy_rng() & 1u) != 0u; };

	for(std::size_t it{}; it < iterations; ++it)
	{
		mount_root_entry entry{};

		// Random lists sizes [0, 6]
		std::size_t const addn = randsz(0, 6);
		std::size_t const rmn = randsz(0, 6);
		std::size_t const escn = randsz(0, 6);

		C::vector<C::u8string> backing{}; // keep storage alive for views
		backing.reserve(addn + rmn + escn + 8);

		auto gen_pat = [&]() -> C::u8string_view
		{
			std::size_t const n = randsz(0, 128);
			C::u8string s{}; s.reserve(n);
			for(std::size_t i{}; i < n; ++i) { s.push_back(rand_char()); }
			backing.emplace_back(::std::move(s));
			return C::u8string_view{backing.back().data(), backing.back().size()};
		};

		for(std::size_t i{}; i < addn; ++i)
		{
			auto v = gen_pat(); auto res = parse_pattern(v);
			entry.add_patterns.emplace_back(v);
			entry.add_automata.emplace_back(build_nfa_from_tokens(res.tokens));
		}
		for(std::size_t i{}; i < rmn; ++i)
		{
			auto v = gen_pat(); auto res = parse_pattern(v);
			entry.rm_patterns.emplace_back(v);
			entry.rm_automata.emplace_back(build_nfa_from_tokens(res.tokens));
		}
		for(std::size_t i{}; i < escn; ++i)
		{
			auto v = gen_pat(); auto res = parse_pattern(v);
			entry.symlink_escape_patterns.emplace_back(v);
			entry.symlink_escape_automata.emplace_back(build_nfa_from_tokens(res.tokens));
		}

		// Random relative path
		C::u8string rel{}; std::size_t const rn = randsz(0, 128);
		rel.reserve(rn);
		for(std::size_t i{}; i < rn; ++i) { rel.push_back(rand_char()); }

		( void )evaluate_path_access(entry, C::u8string_view{rel.data(), rel.size()}, randbool(), randbool(), randbool());
		access_policy pol{}; (void)wasi_rule_allow_open(entry, C::u8string_view{rel.data(), rel.size()}, randbool(), randbool(), randbool(), &pol);
	}
}

// Directory filter fuzz: random names and symlink flags; assert only sizes; crash-only semantics
static void fuzz_dir_filter_random(std::size_t iterations)
{
	fast_io::basic_white_hole_engine<fast_io::u8native_white_hole> entropy_rng{};
	auto rand_char = [&]() -> char8_t
	{
		static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
		std::size_t const idx = static_cast<std::size_t>(entropy_rng()) % (sizeof(pool) / sizeof(pool[0]) - 1);
		return pool[idx];
	};
	auto randsz = [&](std::size_t lo, std::size_t hi) -> std::size_t
	{
		std::size_t const span = (hi - lo + 1);
		return lo + (static_cast<std::size_t>(entropy_rng()) % span);
	};
	auto randbool = [&]() -> bool { return (entropy_rng() & 1u) != 0u; };

	for(std::size_t it{}; it < iterations; ++it)
	{
		mount_root_entry entry{};
		// With some chance add a whitelist so filter does strict include
		if(randbool())
		{
			C::u8string w{}; std::size_t wn = randsz(0, 16); w.reserve(wn);
			for(std::size_t i{}; i < wn; ++i) { w.push_back(rand_char()); }
			auto res = parse_pattern(C::u8string_view{w.data(), w.size()});
			entry.add_patterns.emplace_back(C::u8string_view{w.data(), w.size()});
			entry.add_automata.emplace_back(build_nfa_from_tokens(res.tokens));
		}

		// relative_dir
		C::u8string rel_dir{}; std::size_t const rdn = randsz(0, 32);
		rel_dir.reserve(rdn);
		for(std::size_t i{}; i < rdn; ++i) { rel_dir.push_back(rand_char()); }

		// children
		std::size_t const namesz = randsz(0, 32);
		C::vector<C::u8string> names_backing{};
		names_backing.reserve(namesz);
		C::vector<C::u8string_view> names{};
		names.reserve(namesz);
		for(std::size_t i{}; i < namesz; ++i)
		{
			std::size_t ln = randsz(0, 32);
			C::u8string s{}; s.reserve(ln);
			for(std::size_t j{}; j < ln; ++j) { s.push_back(rand_char()); }
			names_backing.emplace_back(::std::move(s));
			names.emplace_back(C::u8string_view{names_backing.back().data(), names_backing.back().size()});
		}

		C::vector<bool> is_symlink{}; is_symlink.resize(namesz);
		for(std::size_t i{}; i < namesz; ++i) { is_symlink[i] = randbool(); }

		C::vector<bool> out_allowed{};
		wasi_filter_directory_entries(entry,
			C::u8string_view{rel_dir.data(), rel_dir.size()},
			names,
			is_symlink,
			out_allowed);
		require(out_allowed.size() == names.size(), "out_allowed size mismatch in fuzz_dir_filter_random");
	}
}

int main()
{
	std::puts("[RUN] wasi_mount_root.default: basic tests");

	test_parse_valid_invalid();
	test_nfa_match_basic();
	test_access_policy();
	test_dir_filter();
	fuzz_parse_and_match(10000);
	// memory-safety only fuzzers
	fuzz_parse_only(20000);
	fuzz_access_policy_random(10000);
	fuzz_dir_filter_random(10000);

	std::puts("[OK] all tests passed");
	return 0;
}


