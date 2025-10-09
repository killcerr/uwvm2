/*************************************************************
 * UWVM2 test: wasi_mount_dir validate & canonicalize        *
 *************************************************************/

#include <cstdint>
#include <random>
#include <string>
#include <vector>

#ifndef UWVM_MODULE
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
#endif

using uwvm2::utils::container::u8string_view;

// Copy of production logic (kept in-sync intentionally)
static constexpr auto validate_wasidir(u8string_view p) noexcept -> bool {
    if (p.empty()) [[unlikely]] { return false; }
    if (p == u8".") { return true; }
    if (p.front_unchecked() != u8'/') [[unlikely]] { return false; }

    bool prev_slash{};
    std::size_t seg_len{};
    bool seg_only_dots{true};

    for (auto const ch: p) {
        if (ch == u8'/') {
            if (prev_slash) [[unlikely]] { return false; } // forbid '//'
            if (seg_len != 0u) {
                if (seg_only_dots && (seg_len == 1u || seg_len == 2u)) [[unlikely]] { return false; }
            }
            seg_len = 0u;
            seg_only_dots = true;
            prev_slash = true;
        } else {
            if (ch == u8' ' || (::fast_io::char_category::is_dos_path_invalid_character(ch) && ch != u8'.')) [[unlikely]] {
                return false;
            }
            prev_slash = false;
            ++seg_len;
            if (seg_only_dots && ch != u8'.') { seg_only_dots = false; }
        }
    }

    if (seg_len != 0u) {
        if (seg_only_dots && (seg_len == 1u || seg_len == 2u)) [[unlikely]] { return false; }
    }
    return true;
}

static constexpr auto canonicalize_wasidir(u8string_view p)  noexcept -> u8string_view {
    if (p == u8".") { return p; }
    auto n{p.size()};
    while (n > 1u && p.index_unchecked(n - 1u) == u8'/') { --n; }
    return u8string_view{p.data(), n};
}

static constexpr auto starts_with(u8string_view a, u8string_view b)  noexcept -> bool {
    if (a.size() < b.size()) { return false; }
    if (a.size() == b.size()) { return a == b; }
    return (u8string_view{a.data(), b.size()} == b) && (a.index_unchecked(b.size()) == u8'/');
}

// Helpers
static std::mt19937_64 rng{123456789ULL};

static std::u8string gen_valid_path() {
    // allowed chars (no '.') and no backslash; ensure begins with '/'; avoid '//'
    static constexpr char8_t pool[] = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    std::uniform_int_distribution<int> seg_len_dist(1, 8);
    std::uniform_int_distribution<int> seg_cnt_dist(1, 6);
    std::uniform_int_distribution<size_t> ch_dist(0, sizeof(pool) - 2);

    std::u8string s; s.push_back(u8'/');
    int segs = seg_cnt_dist(rng);
    for (int i = 0; i < segs; ++i) {
        int len = seg_len_dist(rng);
        for (int j = 0; j < len; ++j) s.push_back(pool[ch_dist(rng)]);
        if (i + 1 < segs) s.push_back(u8'/');
    }
    // maybe add trailing slash (legal; canonicalize will remove)
    if ((rng() & 3ULL) == 0ULL) s.push_back(u8'/');
    return s;
}

static std::u8string gen_invalid_path_with(char8_t bad) {
    // ensure includes bad char somewhere; if bad == '.', forbid entire '.' case
    std::u8string s; s.push_back(u8'/');
    s.append(u8"seg");
    s.push_back(u8'/');
    s.push_back(bad);
    s.append(u8"xx");
    return s;
}

static std::u8string gen_invalid_not_start_slash() {
    return std::u8string{u8"abc"};
}

int main() {
    using fast_io::io::perr;

    std::size_t total{};
    std::size_t failed{};

    // 1) Dot special-case
    {
        ++total;
        if (!validate_wasidir(u8string_view{u8"."})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"dot case failed\n"); }
    }

    // 2) Valid randoms
    for (int i = 0; i < 2000; ++i) {
        auto s = gen_valid_path();
        ++total;
        if (!validate_wasidir(u8string_view{s.data(), s.size()})) {
            ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"valid reject: ", u8string_view{s.data(), s.size()}, u8"\n");
        }
    }

    // 3) Invalids: dot segments '.', '..'; '//' anywhere; backslash; control chars; not starting '/'
    {
        // dot segments
        {
            static constexpr u8string_view cases[] = {
                u8"/./",
                u8"/a/./b",
                u8"/./a",
                u8"/a/.",
                u8"/../",
                u8"/a/../b",
                u8"/../a",
                u8"/a/..",
                u8"/.",
                u8"/.."
            };
            for (auto c : cases) {
                ++total;
                if (validate_wasidir(c)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"dot-seg bypass: ", c, u8"\n"); }
            }
        }
        // '//'
        for (int i = 0; i < 200; ++i) {
            std::u8string s; s.append(u8"/a//b");
            ++total;
            if (validate_wasidir(u8string_view{s.data(), s.size()})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"double-slash bypass: ", u8string_view{s.data(), s.size()}, u8"\n"); }
        }
        // backslash
        for (int i = 0; i < 200; ++i) {
            auto s = gen_invalid_path_with(u8'\\');
            ++total;
            if (validate_wasidir(u8string_view{s.data(), s.size()})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"backslash bypass: ", u8string_view{s.data(), s.size()}, u8"\n"); }
        }
        // ASCII < 0x20 (e.g. 0x1F)
        for (int i = 0; i < 200; ++i) {
            std::u8string s; s.push_back(u8'/'); s.push_back(static_cast<char8_t>(0x1Fu));
            ++total;
            if (validate_wasidir(u8string_view{s.data(), s.size()})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"ctrl bypass\n"); }
        }
        // not starting '/'
        for (int i = 0; i < 200; ++i) {
            auto s = gen_invalid_not_start_slash();
            ++total;
            if (validate_wasidir(u8string_view{s.data(), s.size()})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"no-leading-slash bypass: ", u8string_view{s.data(), s.size()}, u8"\n"); }
        }
    }

    // 4) Canonicalization & conflict checks
    {
        u8string_view a = canonicalize_wasidir(u8string_view{u8"/s/s/"});
        u8string_view b = canonicalize_wasidir(u8string_view{u8"/s/s"});
        ++total; if (!(a == b)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"canonicalize tail slash failed\n"); }

        u8string_view pfx = canonicalize_wasidir(u8string_view{u8"/a"});
        u8string_view pth = canonicalize_wasidir(u8string_view{u8"/a/b"});
        ++total; if (!starts_with(pth, pfx)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"starts_with detect failed\n"); }
        ++total; if (starts_with(pfx, pth)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"starts_with false positive\n"); }

        u8string_view p1 = canonicalize_wasidir(u8string_view{u8"/ab"});
        ++total; if (starts_with(p1, pfx)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"prefix boundary failed\n"); }

        // tricky cases with spaces: allowed; ensure canonicalization and non-equality boundaries
        u8string_view s1 = canonicalize_wasidir(u8string_view{u8"/s/s "});
        u8string_view s2 = canonicalize_wasidir(u8string_view{u8"/s/s /"});
        ++total; if (!(s1 == s2)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"space canonicalize failed\n"); }
        u8string_view s3 = canonicalize_wasidir(u8string_view{u8"/s/s"});
        ++total; if (s1 == s3) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"space equals no-space false\n"); }
        // prefix with boundary: "/s" should be prefix of "/s/s" but not of "/s/s "
        u8string_view spfx = canonicalize_wasidir(u8string_view{u8"/s"});
        ++total; if (!starts_with(s3, spfx)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"space: spfx missing prefix on /s/s\n"); }
        ++total; if (!starts_with(s1, spfx)) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"space: spfx missing prefix on /s/s <space>\n"); }

        // trailing multi-slashes collapse
        ++total; if (!(canonicalize_wasidir(u8string_view{u8"/t///"}) == u8string_view{u8"/t"})) { ++failed; ::fast_io::io::perr(::fast_io::u8err(), u8"multi trailing slash collapse failed\n"); }
    }

    if (failed) {
        ::fast_io::io::perr(::fast_io::u8err(), u8"wasi_mount_dir_validate_test failed: ", failed, u8" / ", total, u8" cases\n");
        return 1;
    }
    return 0;
}



