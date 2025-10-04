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
	std::mt19937_64 rng{123456789ULL};
	auto rand_char = [&]() -> char8_t
	{
		static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyz/.*?{}\\,";
		std::uniform_int_distribution<std::size_t> d(0, sizeof(pool) / sizeof(pool[0]) - 2);
		return pool[d(rng)];
	};

	for(std::size_t it{}; it < iterations; ++it)
	{
		// build a random small pattern and path
		::uwvm2::utils::container::u8string pat{};
		::uwvm2::utils::container::u8string pth{};
		std::uniform_int_distribution<int> lp(1, 12);
		int n1 = lp(rng), n2 = lp(rng);
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

int main()
{
	std::puts("[RUN] wasi_mount_root.default: basic tests");

	test_parse_valid_invalid();
	test_nfa_match_basic();
	test_access_policy();
	test_dir_filter();
	fuzz_parse_and_match(2000);

	std::puts("[OK] all tests passed");
	return 0;
}


