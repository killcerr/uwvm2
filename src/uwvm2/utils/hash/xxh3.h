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
# include <cstring>
# include <concepts>
# include <bit>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/intrinsics/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::hash
{
    namespace details
    {
        enum class xxh3_width_t : unsigned
        {
            XX3H_64,
            XX3H_128
        };

#if !defined(__SIZEOF_INT128__)
        struct xxh3_128_storage_t
        {
            // For msvc, which targets little-endian, msvc does not have the uint128 extension.
            // Only the uint64_t is guaranteed to be 64 bits.
            static_assert(::std::endian::native == ::std::endian::little);
            ::std::uint64_t low64;
            ::std::uint64_t high64;
        };
#else
        using xxh3_128_storage_t = __uint128_t;
#endif

        template <xxh3_width_t Wid>
            requires (static_cast<unsigned>(Wid) <= static_cast<unsigned>(xxh3_width_t::XX3H_128))
        inline consteval auto get_xxh3_output_struct() noexcept
        {
            if constexpr(Wid == xxh3_width_t::XX3H_64) { return ::std::uint_least64_t{}; }
            else if constexpr(Wid == xxh3_width_t::XX3H_128) { return xxh3_128_storage_t{}; }
        }

        template <xxh3_width_t Wid>
        using xxh3_output_struct = decltype(get_xxh3_output_struct<Wid>());

        inline consteval auto get_currplatform_xxh3_output_struct() noexcept
        {
            if constexpr(sizeof(::std::size_t) > sizeof(::std::uint_least64_t)) { return get_xxh3_output_struct<xxh3_width_t::XX3H_128>(); }
            else
            {
                return get_xxh3_output_struct<xxh3_width_t::XX3H_64>();
            }
        }

        using currplatform_xxh3_output_struct = decltype(get_currplatform_xxh3_output_struct());

        template <typename T>
        inline constexpr ::std::size_t to_size_t(T t) noexcept
        {
            if constexpr(::std::same_as<T, xxh3_128_storage_t>)
            {
                ::std::size_t tmp{t.high64};
                // Avoid ub, loop displacement
                for(unsigned i{}; i != 64u; ++i) { tmp <<= 1u; }
                tmp |= t.low64;
                return tmp;
            }
            else
            {
                return static_cast<::std::size_t>(t);
            }
        }

        inline constexpr ::std::uint_least8_t xxh_read8(::std::byte const* ptr) noexcept
        {
            if UWVM_IF_CONSTEVAL
            {
                auto res{::std::to_integer<::std::uint_least8_t>(*ptr)};
                res &= 0xFFu;
                return res;
            }
            else
            {
#if CHAR_BIT != 8
                auto res{::std::to_integer<::std::uint_least8_t>(*ptr)};
                res &= 0xFFu;
                return res;
#else
                ::std::uint8_t res;
                ::std::memcpy(::std::addressof(res), ptr, sizeof(::std::uint8_t));
                return res;
#endif
            }
        }

        inline constexpr ::std::uint_least16_t xxh_readLE16(::std::byte const* ptr) noexcept
        {
            if UWVM_IF_CONSTEVAL
            {
                ::std::uint_least16_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 2u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least16_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
            }
            else
            {
#if CHAR_BIT != 8
                ::std::uint_least16_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 2u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least16_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
#else
                ::std::uint16_t res;
                ::std::memcpy(::std::addressof(res), ptr, sizeof(::std::uint16_t));
                return ::fast_io::little_endian(res);
#endif
            }
        }

        inline constexpr ::std::uint_least32_t xxh_readLE32(::std::byte const* ptr) noexcept
        {
            if UWVM_IF_CONSTEVAL
            {
                ::std::uint_least32_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 4u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least32_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
            }
            else
            {
#if CHAR_BIT != 8
                ::std::uint_least32_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 4u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least32_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
#else
                ::std::uint32_t res;
                ::std::memcpy(::std::addressof(res), ptr, sizeof(::std::uint32_t));
                return ::fast_io::little_endian(res);
#endif
            }
        }

        inline constexpr ::std::uint_least64_t xxh_readLE64(::std::byte const* ptr) noexcept
        {
            if UWVM_IF_CONSTEVAL
            {
                ::std::uint_least64_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 8u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least64_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
            }
            else
            {
#if CHAR_BIT != 8
                ::std::uint_least64_t res{};
                unsigned shf{};
                for(unsigned i{}; i != 8u; ++i)
                {
                    auto curr{::std::to_integer<::std::uint_least64_t>(ptr[i])};
                    curr &= 0xFFu;

                    res |= curr << shf;
                    shf += 8u;
                }
                return res;
#else
                ::std::uint64_t res;
                ::std::memcpy(::std::addressof(res), ptr, sizeof(::std::uint64_t));
                return ::fast_io::little_endian(res);
#endif
            }
        }

        alignas(64uz) inline constexpr ::std::byte xxh3_kSecret[192u]{
            static_cast<::std::byte>(0xb8), static_cast<::std::byte>(0xfe), static_cast<::std::byte>(0x6c), static_cast<::std::byte>(0x39),
            static_cast<::std::byte>(0x23), static_cast<::std::byte>(0xa4), static_cast<::std::byte>(0x4b), static_cast<::std::byte>(0xbe),
            static_cast<::std::byte>(0x7c), static_cast<::std::byte>(0x01), static_cast<::std::byte>(0x81), static_cast<::std::byte>(0x2c),
            static_cast<::std::byte>(0xf7), static_cast<::std::byte>(0x21), static_cast<::std::byte>(0xad), static_cast<::std::byte>(0x1c),
            static_cast<::std::byte>(0xde), static_cast<::std::byte>(0xd4), static_cast<::std::byte>(0x6d), static_cast<::std::byte>(0xe9),
            static_cast<::std::byte>(0x83), static_cast<::std::byte>(0x90), static_cast<::std::byte>(0x97), static_cast<::std::byte>(0xdb),
            static_cast<::std::byte>(0x72), static_cast<::std::byte>(0x40), static_cast<::std::byte>(0xa4), static_cast<::std::byte>(0xa4),
            static_cast<::std::byte>(0xb7), static_cast<::std::byte>(0xb3), static_cast<::std::byte>(0x67), static_cast<::std::byte>(0x1f),
            static_cast<::std::byte>(0xcb), static_cast<::std::byte>(0x79), static_cast<::std::byte>(0xe6), static_cast<::std::byte>(0x4e),
            static_cast<::std::byte>(0xcc), static_cast<::std::byte>(0xc0), static_cast<::std::byte>(0xe5), static_cast<::std::byte>(0x78),
            static_cast<::std::byte>(0x82), static_cast<::std::byte>(0x5a), static_cast<::std::byte>(0xd0), static_cast<::std::byte>(0x7d),
            static_cast<::std::byte>(0xcc), static_cast<::std::byte>(0xff), static_cast<::std::byte>(0x72), static_cast<::std::byte>(0x21),
            static_cast<::std::byte>(0xb8), static_cast<::std::byte>(0x08), static_cast<::std::byte>(0x46), static_cast<::std::byte>(0x74),
            static_cast<::std::byte>(0xf7), static_cast<::std::byte>(0x43), static_cast<::std::byte>(0x24), static_cast<::std::byte>(0x8e),
            static_cast<::std::byte>(0xe0), static_cast<::std::byte>(0x35), static_cast<::std::byte>(0x90), static_cast<::std::byte>(0xe6),
            static_cast<::std::byte>(0x81), static_cast<::std::byte>(0x3a), static_cast<::std::byte>(0x26), static_cast<::std::byte>(0x4c),
            static_cast<::std::byte>(0x3c), static_cast<::std::byte>(0x28), static_cast<::std::byte>(0x52), static_cast<::std::byte>(0xbb),
            static_cast<::std::byte>(0x91), static_cast<::std::byte>(0xc3), static_cast<::std::byte>(0x00), static_cast<::std::byte>(0xcb),
            static_cast<::std::byte>(0x88), static_cast<::std::byte>(0xd0), static_cast<::std::byte>(0x65), static_cast<::std::byte>(0x8b),
            static_cast<::std::byte>(0x1b), static_cast<::std::byte>(0x53), static_cast<::std::byte>(0x2e), static_cast<::std::byte>(0xa3),
            static_cast<::std::byte>(0x71), static_cast<::std::byte>(0x64), static_cast<::std::byte>(0x48), static_cast<::std::byte>(0x97),
            static_cast<::std::byte>(0xa2), static_cast<::std::byte>(0x0d), static_cast<::std::byte>(0xf9), static_cast<::std::byte>(0x4e),
            static_cast<::std::byte>(0x38), static_cast<::std::byte>(0x19), static_cast<::std::byte>(0xef), static_cast<::std::byte>(0x46),
            static_cast<::std::byte>(0xa9), static_cast<::std::byte>(0xde), static_cast<::std::byte>(0xac), static_cast<::std::byte>(0xd8),
            static_cast<::std::byte>(0xa8), static_cast<::std::byte>(0xfa), static_cast<::std::byte>(0x76), static_cast<::std::byte>(0x3f),
            static_cast<::std::byte>(0xe3), static_cast<::std::byte>(0x9c), static_cast<::std::byte>(0x34), static_cast<::std::byte>(0x3f),
            static_cast<::std::byte>(0xf9), static_cast<::std::byte>(0xdc), static_cast<::std::byte>(0xbb), static_cast<::std::byte>(0xc7),
            static_cast<::std::byte>(0xc7), static_cast<::std::byte>(0x0b), static_cast<::std::byte>(0x4f), static_cast<::std::byte>(0x1d),
            static_cast<::std::byte>(0x8a), static_cast<::std::byte>(0x51), static_cast<::std::byte>(0xe0), static_cast<::std::byte>(0x4b),
            static_cast<::std::byte>(0xcd), static_cast<::std::byte>(0xb4), static_cast<::std::byte>(0x59), static_cast<::std::byte>(0x31),
            static_cast<::std::byte>(0xc8), static_cast<::std::byte>(0x9f), static_cast<::std::byte>(0x7e), static_cast<::std::byte>(0xc9),
            static_cast<::std::byte>(0xd9), static_cast<::std::byte>(0x78), static_cast<::std::byte>(0x73), static_cast<::std::byte>(0x64),
            static_cast<::std::byte>(0xea), static_cast<::std::byte>(0xc5), static_cast<::std::byte>(0xac), static_cast<::std::byte>(0x83),
            static_cast<::std::byte>(0x34), static_cast<::std::byte>(0xd3), static_cast<::std::byte>(0xeb), static_cast<::std::byte>(0xc3),
            static_cast<::std::byte>(0xc5), static_cast<::std::byte>(0x81), static_cast<::std::byte>(0xa0), static_cast<::std::byte>(0xff),
            static_cast<::std::byte>(0xfa), static_cast<::std::byte>(0x13), static_cast<::std::byte>(0x63), static_cast<::std::byte>(0xeb),
            static_cast<::std::byte>(0x17), static_cast<::std::byte>(0x0d), static_cast<::std::byte>(0xdd), static_cast<::std::byte>(0x51),
            static_cast<::std::byte>(0xb7), static_cast<::std::byte>(0xf0), static_cast<::std::byte>(0xda), static_cast<::std::byte>(0x49),
            static_cast<::std::byte>(0xd3), static_cast<::std::byte>(0x16), static_cast<::std::byte>(0x55), static_cast<::std::byte>(0x26),
            static_cast<::std::byte>(0x29), static_cast<::std::byte>(0xd4), static_cast<::std::byte>(0x68), static_cast<::std::byte>(0x9e),
            static_cast<::std::byte>(0x2b), static_cast<::std::byte>(0x16), static_cast<::std::byte>(0xbe), static_cast<::std::byte>(0x58),
            static_cast<::std::byte>(0x7d), static_cast<::std::byte>(0x47), static_cast<::std::byte>(0xa1), static_cast<::std::byte>(0xfc),
            static_cast<::std::byte>(0x8f), static_cast<::std::byte>(0xf8), static_cast<::std::byte>(0xb8), static_cast<::std::byte>(0xd1),
            static_cast<::std::byte>(0x7a), static_cast<::std::byte>(0xd0), static_cast<::std::byte>(0x31), static_cast<::std::byte>(0xce),
            static_cast<::std::byte>(0x45), static_cast<::std::byte>(0xcb), static_cast<::std::byte>(0x3a), static_cast<::std::byte>(0x8f),
            static_cast<::std::byte>(0x95), static_cast<::std::byte>(0x16), static_cast<::std::byte>(0x04), static_cast<::std::byte>(0x28),
            static_cast<::std::byte>(0xaf), static_cast<::std::byte>(0xd7), static_cast<::std::byte>(0xfb), static_cast<::std::byte>(0xca),
            static_cast<::std::byte>(0xbb), static_cast<::std::byte>(0x4b), static_cast<::std::byte>(0x40), static_cast<::std::byte>(0x7e)};

        inline constexpr ::std::uint_least32_t xxh_prime32_1{0x9E3779B1U}; /*!< 0b10011110001101110111100110110001 */
        inline constexpr ::std::uint_least32_t xxh_prime32_2{0x85EBCA77U}; /*!< 0b10000101111010111100101001110111 */
        inline constexpr ::std::uint_least32_t xxh_prime32_3{0xC2B2AE3DU}; /*!< 0b11000010101100101010111000111101 */
        inline constexpr ::std::uint_least32_t xxh_prime32_4{0x27D4EB2FU}; /*!< 0b00100111110101001110101100101111 */
        inline constexpr ::std::uint_least32_t xxh_prime32_5{0x165667B1U}; /*!< 0b00010110010101100110011110110001 */

        inline constexpr ::std::uint_least64_t xxh_prime64_1{0x9E3779B185EBCA87ULL}; /*!< 0b1001111000110111011110011011000110000101111010111100101010000111 */
        inline constexpr ::std::uint_least64_t xxh_prime64_2{0xC2B2AE3D27D4EB4FULL}; /*!< 0b1100001010110010101011100011110100100111110101001110101101001111 */
        inline constexpr ::std::uint_least64_t xxh_prime64_3{0x165667B19E3779F9ULL}; /*!< 0b0001011001010110011001111011000110011110001101110111100111111001 */
        inline constexpr ::std::uint_least64_t xxh_prime64_4{0x85EBCA77C2B2AE63ULL}; /*!< 0b1000010111101011110010100111011111000010101100101010111001100011 */
        inline constexpr ::std::uint_least64_t xxh_prime64_5{0x27D4EB2F165667C5ULL}; /*!< 0b0010011111010100111010110010111100010110010101100110011111000101 */

        inline constexpr ::std::uint_least64_t prime_mx1{0x165667919E3779F9ULL}; /*!< 0b0001011001010110011001111001000110011110001101110111100111111001 */
        inline constexpr ::std::uint_least64_t prime_mx2{0x9FB21C651E98DF25ULL}; /*!< 0b1001111110110010000111000110010100011110100110001101111100100101 */

        inline constexpr ::std::uint_least64_t xxh64_avalanche(::std::uint_least64_t hash) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            hash ^= hash >> 33u;
            hash *= xxh_prime64_2;
            if constexpr(dig64 != 64) { hash &= 0xFFFF'FFFF'FFFF'FFFFu; }
            hash ^= hash >> 29u;
            hash *= xxh_prime64_3;
            if constexpr(dig64 != 64) { hash &= 0xFFFF'FFFF'FFFF'FFFFu; }
            hash ^= hash >> 32u;

            return hash;
        }

        inline constexpr ::std::uint_least64_t xxh_xorshift64(::std::uint_least64_t val, unsigned shf) noexcept
        {
            [[assume(0u <= shf && shf < 64u)]];

            return val ^ (val >> shf);
        }

        inline constexpr ::std::uint_least64_t xxh3_avalanche(::std::uint_least64_t hash) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            hash = xxh_xorshift64(hash, 37u);
            hash *= prime_mx1;
            if constexpr(dig64 != 64) { hash &= 0xFFFF'FFFF'FFFF'FFFFu; }
            hash = xxh_xorshift64(hash, 32u);
            return hash;
        }

        inline constexpr ::std::uint_least64_t xxh_rotl64(::std::uint_least64_t val, unsigned shf) noexcept
        {
            if constexpr(::std::numeric_limits<::std::uint_least64_t>::digits == 64) { return ::std::rotl(val, static_cast<int>(shf)); }
            else
            {
                constexpr unsigned bits{64u};
                unsigned const s{shf & (bits - 1u)};
                return ((val << s) & 0xFFFF'FFFF'FFFF'FFFFu) | ((val & 0xFFFF'FFFF'FFFF'FFFFu) >> (bits - s));
            }
        }

        inline constexpr ::std::uint_least64_t xxh3_rrmxmx(::std::uint_least64_t h64, ::std::uint_least64_t len) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            /* this mix is inspired by Pelle Evensen's rrmxmx */
            h64 ^= xxh_rotl64(h64, 49u) ^ xxh_rotl64(h64, 24u);
            h64 *= prime_mx2;
            if constexpr(dig64 != 64) { h64 &= 0xFFFF'FFFF'FFFF'FFFFu; }
            h64 ^= (h64 >> 35u) + len;
            h64 *= prime_mx2;
            if constexpr(dig64 != 64) { h64 &= 0xFFFF'FFFF'FFFF'FFFFu; }
            return xxh_xorshift64(h64, 28u);
        }

        inline constexpr ::std::uint_least16_t xxh_swap16(::std::uint_least16_t val) noexcept
        {
            constexpr auto dig16{::std::numeric_limits<::std::uint_least16_t>::digits};

            if constexpr(dig16 == 16) { return ::fast_io::byte_swap(val); }
            else
            {
                ::std::uint_least16_t res{};
                res |= (val & 0xFF00u) >> 8u;
                res |= (val & 0x00FFu) << 8u;
                return res;
            }
        }

        inline constexpr ::std::uint_least32_t xxh_swap32(::std::uint_least32_t val) noexcept
        {
            constexpr auto dig32{::std::numeric_limits<::std::uint_least32_t>::digits};

            if constexpr(dig32 == 32) { return ::fast_io::byte_swap(val); }
            else
            {
                ::std::uint_least32_t res{};
                res |= (val & 0xFF00'0000u) >> 24u;
                res |= (val & 0x00FF'0000u) >> 8u;
                res |= (val & 0x0000'FF00u) << 8u;
                res |= (val & 0x0000'00FFu) << 24u;
                return res;
            }
        }

        inline constexpr ::std::uint_least64_t xxh_swap64(::std::uint_least64_t val) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 == 64) { return ::fast_io::byte_swap(val); }
            else
            {
                ::std::uint_least64_t res{};
                res |= (val & 0xFF00'0000'0000'0000u) >> 56u;
                res |= (val & 0x00FF'0000'0000'0000u) >> 40u;
                res |= (val & 0x0000'FF00'0000'0000u) >> 24u;
                res |= (val & 0x0000'00FF'0000'0000u) >> 8u;
                res |= (val & 0x0000'0000'FF00'0000u) << 8u;
                res |= (val & 0x0000'0000'00FF'0000u) << 24u;
                res |= (val & 0x0000'0000'0000'FF00u) << 40u;
                res |= (val & 0x0000'0000'0000'00FFu) << 56u;
                return res;
            }
        }

        inline constexpr ::std::uint_least64_t xxh3_mul128_fold64(::std::uint_least64_t a, ::std::uint_least64_t b) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 == 64)
            {
                ::std::uint_least64_t h;
                auto const l{::fast_io::intrinsics::umul(a, b, h)};
                return l ^ h;
            }
            else
            {
#ifdef __SIZEOF_INT128__
                __uint128_t const res{static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b)};
                auto const l{static_cast<::std::uint_least64_t>(res) & 0xFFFF'FFFF'FFFF'FFFFu};
                auto const h{static_cast<::std::uint_least64_t>(res >> 64u) & 0xFFFF'FFFF'FFFF'FFFFu};
                return l ^ h;
#else
                ::std::uint_least64_t const a_lo{a & 0xFFFF'FFFFu};
                ::std::uint_least64_t const a_hi{(a >> 32u) & 0xFFFF'FFFFu};
                ::std::uint_least64_t const b_lo{b & 0xFFFF'FFFFu};
                ::std::uint_least64_t const b_hi{(b >> 32u) & 0xFFFF'FFFFu};

                ::std::uint_least64_t const p0{a_lo * b_lo};
                ::std::uint_least64_t const p1{a_lo * b_hi};
                ::std::uint_least64_t const p2{a_hi * b_lo};
                ::std::uint_least64_t const p3{a_hi * b_hi};

                ::std::uint_least64_t const mid{(p0 >> 32u) + (p1 & 0xFFFF'FFFFu) + (p2 & 0xFFFF'FFFFu)};
                ::std::uint_least64_t const low{(p0 & 0xFFFF'FFFFu) | ((mid << 32u) & 0xFFFF'FFFF'FFFF'FFFFu)};

                ::std::uint_least64_t const high{p3 + (p1 >> 32u) + (p2 >> 32u) + (mid >> 32u)};

                return low ^ high;
#endif
            }
        }

        inline constexpr ::std::uint_least64_t
            xxh3_mix16B(::std::byte const* __restrict input, ::std::byte const* __restrict secret, ::std::uint_least64_t seed64) noexcept
        {
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 != 64) { seed64 &= 0xFFFF'FFFF'FFFF'FFFFu; }

            ::std::uint_least64_t const input_lo{xxh_readLE64(input)};
            ::std::uint_least64_t const input_hi{xxh_readLE64(input + 8u)};
            ::std::uint_least64_t f1{input_lo ^ (xxh_readLE64(secret) + seed64)};

            if constexpr(dig64 != 64) { f1 &= 0xFFFF'FFFF'FFFF'FFFFu; }

            ::std::uint_least64_t f2{input_hi ^ (xxh_readLE64(secret + 8u) - seed64)};

            if constexpr(dig64 != 64) { f2 &= 0xFFFF'FFFF'FFFF'FFFFu; }

            return xxh3_mul128_fold64(f1, f2);
        }

        inline constexpr ::std::uint_least64_t xxh3_mix2_accs(::std::uint_least64_t const* __restrict acc, ::std::byte const* __restrict secret) noexcept
        {
            return xxh3_mul128_fold64(acc[0u] ^ xxh_readLE64(secret), acc[1u] ^ xxh_readLE64(secret + 8u));
        }

        inline constexpr ::std::uint_least64_t
            xxh3_merge_accs(::std::uint_least64_t const* __restrict acc, ::std::byte const* __restrict secret, ::std::uint_least64_t start) noexcept
        {
            ::std::uint_least64_t result64{start};
            for(unsigned i{}; i != 4u; ++i) { result64 += xxh3_mix2_accs(acc + 2u * i, secret + 16u * i); }

            if constexpr(::std::numeric_limits<::std::uint_least64_t>::digits != 64) { result64 &= 0xFFFF'FFFF'FFFF'FFFFu; }

            return xxh3_avalanche(result64);
        }

        inline constexpr ::std::uint_least64_t
            xxh3_finalize_long_64b(::std::uint_least64_t const* __restrict acc, ::std::byte const* __restrict secret, ::std::uint_least64_t len) noexcept
        {
            auto start{len * xxh_prime64_1};

            if constexpr(::std::numeric_limits<::std::uint_least64_t>::digits != 64) { start &= 0xFFFF'FFFF'FFFF'FFFFu; }

            return xxh3_merge_accs(acc, secret + 11u, start);
        }

        inline constexpr ::std::uint_least64_t xxh_mult32to64_add64(::std::uint_least64_t lhs, ::std::uint_least64_t rhs, ::std::uint_least64_t acc) noexcept
        {
            auto res{(lhs & 0xFFFF'FFFFu) * (rhs & 0xFFFF'FFFFu) + acc};
            if constexpr(::std::numeric_limits<::std::uint_least64_t>::digits != 64) { res &= 0xFFFF'FFFF'FFFF'FFFFu; }
            return res;
        }

        inline constexpr ::std::uint_least64_t
            xxh3_len_1to3_64b(::std::byte const* __restrict input, ::std::size_t len, ::std::byte const* __restrict secret, ::std::uint_least64_t seed) noexcept
        {
            [[assume(input != nullptr)]];
            [[assume(1uz <= len && len <= 3uz)]];
            [[assume(secret != nullptr)]];

            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            /*
             * len = 1: combined = { input[0], 0x01, input[0], input[0] }
             * len = 2: combined = { input[1], 0x02, input[0], input[1] }
             * len = 3: combined = { input[2], 0x03, input[0], input[1] }
             */

            if constexpr(dig64 != 64) { seed &= 0xFFFF'FFFF'FFFF'FFFFu; }

            auto const c1{xxh_read8(input)};
            auto const c2{xxh_read8(input + (len >> 1u))};
            auto const c3{xxh_read8(input + (len - 1u))};
            auto const combined{static_cast<::std::uint_least32_t>((static_cast<::std::uint_least32_t>(c1) << 16u) |
                                                                   (static_cast<::std::uint_least32_t>(c2) << 24u) | static_cast<::std::uint_least32_t>(c3) |
                                                                   (static_cast<::std::uint_least32_t>(len) << 8u))};
            auto bitflip{static_cast<::std::uint_least64_t>((xxh_readLE32(secret) ^ xxh_readLE32(secret + 4u)) + seed)};
            if constexpr(dig64 != 64) { bitflip &= 0xFFFF'FFFF'FFFF'FFFFu; }
            auto const keyed{static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(combined) ^ bitflip)};
            return xxh64_avalanche(keyed);
        }

        inline constexpr ::std::uint_least64_t
            xxh3_len_4to8_64b(::std::byte const* __restrict input, ::std::size_t len, ::std::byte const* __restrict secret, ::std::uint_least64_t seed) noexcept
        {
            [[assume(input != nullptr)]];
            [[assume(secret != nullptr)]];
            [[assume(4uz <= len && len <= 8uz)]];

            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 != 64) { seed &= 0xFFFF'FFFF'FFFF'FFFFu; }

            seed ^= static_cast<::std::uint_least64_t>(xxh_swap32(static_cast<::std::uint_least32_t>(seed))) << 32u;

            ::std::uint_least32_t const input1{xxh_readLE32(input)};
            ::std::uint_least32_t const input2{xxh_readLE32(input + (len - 4u))};
            ::std::uint_least64_t bitflip{(xxh_readLE64(secret + 8u) ^ xxh_readLE64(secret + 16u)) - seed};
            if constexpr(dig64 != 64) { bitflip &= 0xFFFF'FFFF'FFFF'FFFFu; }
            ::std::uint_least64_t const input64{input2 + ((static_cast<::std::uint_least64_t>(input1)) << 32u)};
            ::std::uint_least64_t const keyed{input64 ^ bitflip};
            return xxh3_rrmxmx(keyed, len);
        }

        inline constexpr ::std::uint_least64_t xxh3_len_9to16_64b(::std::byte const* __restrict input,
                                                                  ::std::size_t len,
                                                                  ::std::byte const* __restrict secret,
                                                                  ::std::uint_least64_t seed) noexcept
        {
            [[assume(input != nullptr)]];
            [[assume(secret != nullptr)]];
            [[assume(9uz <= len && len <= 16uz)]];

            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 != 64) { seed &= 0xFFFF'FFFF'FFFF'FFFFu; }

            ::std::uint_least64_t bitflip1{(xxh_readLE64(secret + 24u) ^ xxh_readLE64(secret + 32u)) + seed};
            if constexpr(dig64 != 64) { bitflip1 &= 0xFFFF'FFFF'FFFF'FFFFu; }
            ::std::uint_least64_t bitflip2{(xxh_readLE64(secret + 40u) ^ xxh_readLE64(secret + 48u)) - seed};
            if constexpr(dig64 != 64) { bitflip2 &= 0xFFFF'FFFF'FFFF'FFFFu; }
            ::std::uint_least64_t const input_lo{xxh_readLE64(input) ^ bitflip1};
            ::std::uint_least64_t const input_hi{xxh_readLE64(input + (len - 8u)) ^ bitflip2};
            ::std::uint_least64_t acc{len + xxh_swap64(input_lo) + input_hi + xxh3_mul128_fold64(input_lo, input_hi)};
            if constexpr(dig64 != 64) { acc &= 0xFFFF'FFFF'FFFF'FFFFu; }
            return xxh3_avalanche(acc);
        }

        inline constexpr ::std::uint_least64_t xxh3_len_0to16_64b(::std::byte const* __restrict input,
                                                                  ::std::size_t len,
                                                                  ::std::byte const* __restrict secret,
                                                                  ::std::uint_least64_t seed) noexcept
        {
            [[assume(len <= 16uz)]];
            if(len > 8uz) { return xxh3_len_9to16_64b(input, len, secret, seed); }
            else if(len >= 4uz) { return xxh3_len_4to8_64b(input, len, secret, seed); }
            else if(len) { return xxh3_len_1to3_64b(input, len, secret, seed); }
            else
            {
                return xxh64_avalanche(seed ^ (xxh_readLE64(secret + 56u) ^ xxh_readLE64(secret + 64u)));
            }
        }

        inline constexpr ::std::uint_least64_t xxh3_len_17to128_64b(::std::byte const* __restrict input,
                                                                    ::std::size_t len,
                                                                    ::std::byte const* __restrict secret,
                                                                    [[maybe_unused]] ::std::size_t secretLen,
                                                                    ::std::uint_least64_t seed) noexcept
        {
            [[assume(secretLen >= 136uz)]];
            [[assume(16uz < len && len <= 128uz)]];

            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 != 64) { seed &= 0xFFFF'FFFF'FFFF'FFFFu; }

            ::std::uint_least64_t acc{len * xxh_prime64_1};

            if constexpr(dig64 != 64) { acc &= 0xFFFF'FFFF'FFFF'FFFFu; }

            if(len > 32uz)
            {
                if(len > 64uz)
                {
                    if(len > 96uz)
                    {
                        acc += xxh3_mix16B(input + 48u, secret + 96u, seed);
                        acc += xxh3_mix16B(input + (len - 64uz), secret + 112u, seed);
                    }
                    acc += xxh3_mix16B(input + 32u, secret + 64u, seed);
                    acc += xxh3_mix16B(input + (len - 48uz), secret + 80u, seed);
                }
                acc += xxh3_mix16B(input + 16u, secret + 32u, seed);
                acc += xxh3_mix16B(input + (len - 32uz), secret + 48u, seed);
            }
            acc += xxh3_mix16B(input, secret, seed);
            acc += xxh3_mix16B(input + (len - 16uz), secret + 16u, seed);
            if constexpr(dig64 != 64) { acc &= 0xFFFF'FFFF'FFFF'FFFFu; }
            return xxh3_avalanche(acc);
        }

        inline constexpr ::std::uint_least64_t xxh3_len_129to240_64b(::std::byte const* __restrict input,
                                                                     ::std::size_t len,
                                                                     ::std::byte const* __restrict secret,
                                                                     [[maybe_unused]] ::std::size_t secretSize,
                                                                     ::std::uint_least64_t seed) noexcept
        {
            [[assume(secretSize >= 136uz)]];
            [[assume(128uz < len && len <= 240uz)]];

            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            if constexpr(dig64 != 64) { seed &= 0xFFFF'FFFF'FFFF'FFFFu; }
            ::std::uint_least64_t acc{len * xxh_prime64_1};
            unsigned const nbRounds{static_cast<unsigned>(len) / 16u};
            for(unsigned i{}; i != 8u; ++i) { acc += xxh3_mix16B(input + (16u * i), secret + (16u * i), seed); }
            /* last bytes */
            ::std::uint_least64_t acc_end{xxh3_mix16B(input + (len - 16u), secret + (136u - 17u), seed)};

            acc = xxh3_avalanche(acc);

            for(unsigned i{8u}; i < nbRounds; ++i) { acc_end += xxh3_mix16B(input + (16u * i), secret + ((16u * (i - 8u)) + 3u), seed); }
            return xxh3_avalanche(acc + acc_end);
        }

        inline constexpr void
            xxh3_accumulate_512(::std::byte* __restrict acc, ::std::byte const* __restrict input, ::std::byte const* __restrict secret) noexcept
        {
#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__AVX512F__)
            using i64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int64_t;
            using u64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x16simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint32_t;
            using i32x16simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int32_t;
            using c8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = char;
            using u8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int8_t;

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            using u8x64simd_may_alias_ptr UWVM_GNU_MAY_ALIAS = u8x64simd*;
            auto const xacc{reinterpret_cast<u8x64simd_may_alias_ptr>(acc_64aligned)};

            u8x64simd data_vec;
            ::std::memcpy(::std::addressof(data_vec), input, sizeof(u8x64simd));

            u8x64simd key_vec;
            ::std::memcpy(::std::addressof(key_vec), input, sizeof(u8x64simd));

            auto const data_key{data_vec ^ key_vec};

            u8x64simd data_key_lo;
# if UWVM_HAS_BUILTIN(__builtin_ia32_psrlqi512)  // Clang
            data_key_lo = ::std::bit_cast<u8x64simd>(__builtin_ia32_psrlqi512(::std::bit_cast<i64x8simd>(data_key), 32));
# elif UWVM_HAS_BUILTIN(__builtin_ia32_psrlqi512_mask)  // GCC
            data_key_lo = ::std::bit_cast<u8x64simd>(__builtin_ia32_psrlqi512_mask(::std::bit_cast<i64x8simd>(data_key), 32u, i64x8simd{}, UINT8_MAX));
# else
#  error "missing instruction"
# endif

            u8x64simd product;
# if UWVM_HAS_BUILTIN(__builtin_ia32_pmuludq512)  // Clang
            product = ::std::bit_cast<u8x64simd>(__builtin_ia32_pmuludq512(::std::bit_cast<i32x16simd>(data_key), ::std::bit_cast<i32x16simd>(data_key_lo)));
# elif UWVM_HAS_BUILTIN(__builtin_ia32_pmuludq512_mask)  // GCC
            product = ::std::bit_cast<u8x64simd>(
                __builtin_ia32_pmuludq512_mask(::std::bit_cast<i32x16simd>(data_key), ::std::bit_cast<i32x16simd>(data_key_lo), i64x8simd{}, UINT8_MAX));
# else
#  error "missing instruction"
# endif

            u8x64simd data_swap;
# if UWVM_HAS_BUILTIN(__builtin_ia32_pshufd512)  // Clang
            data_swap = ::std::bit_cast<u8x64simd>(
                __builtin_ia32_pshufd512(::std::bit_cast<i32x16simd>(data_vec), static_cast<int>(1u << 6u | 0u << 4u | 3u << 2u | 2u)));
# elif UWVM_HAS_BUILTIN(__builtin_ia32_pshufd512_mask)  // GCC
            data_swap = ::std::bit_cast<u8x64simd>(__builtin_ia32_pshufd512_mask(::std::bit_cast<i32x16simd>(data_vec),
                                                                                 static_cast<int>(1u << 6u | 0u << 4u | 3u << 2u | 2u),
                                                                                 i32x16simd{},
                                                                                 UINT16_MAX));
# else
#  error "missing instruction"
# endif

            auto const sum{::std::bit_cast<u8x64simd>(::std::bit_cast<u64x8simd>(*xacc) + ::std::bit_cast<u64x8simd>(data_swa))};

            *xacc = ::std::bit_cast<u8x64simd>(::std::bit_cast<u64x8simd>(product), ::std::bit_cast<u64x8simd>(sum));

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__AVX2__)

            using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
            using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
            using i32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int32_t;
            using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
            using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            using u8x32simd_may_alias_ptr UWVM_GNU_MAY_ALIAS = u8x32simd*;
            auto const xacc{reinterpret_cast<u8x32simd_may_alias_ptr>(acc_64aligned)};

            for(unsigned i{}; i != 2u; ++i)
            {
                u8x32simd data_vec;
                ::std::memcpy(::std::addressof(data_vec), input, sizeof(u8x32simd));
                input += sizeof(u8x32simd);

                u8x32simd key_vec;
                ::std::memcpy(::std::addressof(key_vec), secret, sizeof(u8x32simd));
                secret += sizeof(u8x32simd);

                auto const data_key{data_vec ^ key_vec};

# if UWVM_HAS_BUILTIN(__builtin_ia32_psrlqi256)
                auto const data_key_lo{::std::bit_cast<u8x32simd>(__builtin_ia32_psrlqi256(::std::bit_cast<u64x4simd>(data_key), 32))};
# else
#  error "missing instruction"
# endif

# if UWVM_HAS_BUILTIN(__builtin_ia32_pmuludq256)
                auto const product{
                    ::std::bit_cast<u8x32simd>(__builtin_ia32_pmuludq256(::std::bit_cast<i32x8simd>(data_key), ::std::bit_cast<i32x8simd>(data_key_lo)))};
# else
#  error "missing instruction"
# endif

# if UWVM_HAS_BUILTIN(__builtin_ia32_pshufd256)
                auto const data_swap{::std::bit_cast<u8x32simd>(
                    __builtin_ia32_pshufd256(::std::bit_cast<i32x8simd>(data_vec), static_cast<int>(1u << 6u | 0u << 4u | 3u << 2u | 2u)))};
# else
#  error "missing instruction"
# endif

                auto const sum{::std::bit_cast<u8x32simd>(::std::bit_cast<u64x4simd>(*xacc) + ::std::bit_cast<u64x4simd>(data_swap))};

                *xacc = ::std::bit_cast<u8x32simd>(::std::bit_cast<u64x4simd>(product) + ::std::bit_cast<u64x4simd>(sum));

                ++xacc;
            }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__SSE2__)
            using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
            using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
            using i32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int32_t;
            using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
            using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            using u8x16simd_may_alias_ptr UWVM_GNU_MAY_ALIAS = u8x16simd*;
            auto const xacc{reinterpret_cast<u8x16simd_may_alias_ptr>(acc_64aligned)};

            for(unsigned i{}; i != 4u; ++i)
            {
                u8x16simd data_vec;
                ::std::memcpy(::std::addressof(data_vec), input, sizeof(u8x16simd));
                input += sizeof(u8x16simd);

                u8x16simd key_vec;
                ::std::memcpy(::std::addressof(key_vec), secret, sizeof(u8x16simd));
                secret += sizeof(u8x16simd);

                auto const data_key{data_vec ^ key_vec};

# if UWVM_HAS_BUILTIN(__builtin_ia32_pshufd)
                auto const data_key_lo{__builtin_ia32_pshufd(::std::bit_cast<i32x4simd>(data_key), static_cast<int>(0u << 6u | 3u << 4u | 0u << 2u | 1u))};
# else
#  error "missing instruction"
# endif

# if UWVM_HAS_BUILTIN(__builtin_ia32_pmuludq128)
                auto const product{__builtin_ia32_pmuludq128(::std::bit_cast<i32x4simd>(data_key), ::std::bit_cast<i32x4simd>(data_key_lo))};
# else
#  error "missing instruction"
# endif

# if UWVM_HAS_BUILTIN(__builtin_ia32_pshufd)
                auto const data_swap{__builtin_ia32_pshufd(::std::bit_cast<i32x4simd>(data_vec), static_cast<int>(1u << 6u | 0u << 4u | 3u << 2u | 2u))};
# else
#  error "missing instruction"
# endif

                auto const sum{::std::bit_cast<u64x2simd>(*xacc) + ::std::bit_cast<u64x2simd>(data_swap)};

                *xacc = ::std::bit_cast<u64x2simd>(product) + ::std::bit_cast<u64x2simd>(sum);

                ++xacc;
            }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                           \
    ((defined(UWVM_ENABLE_SME_SVE_STREAM_MODE) && defined(__ARM_FEATURE_SME)) && !defined(__ARM_FEATURE_SVE))

            [=] __arm_locally_streaming() constexpr noexcept -> void
            {
                auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
                auto xacc{reinterpret_cast<::std::uint64_t*>(acc_64aligned)};
                using uint64_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint64_t const*;
                auto xinput{reinterpret_cast<uint64_t_const_may_alias_ptr>(input)};
                auto xsecret{reinterpret_cast<uint64_t_const_may_alias_ptr>(secret)};
                auto const kSwap{::uwvm2::utils::intrinsics::arm_sve::sveor_n_u64_z(::uwvm2::utils::intrinsics::arm_sve::svptrue_b64(),
                                                                                    ::uwvm2::utils::intrinsics::arm_sve::svindex_u64(0u, 1u),
                                                                                    1u)};
                ::std::uint64_t element_count{::uwvm2::utils::intrinsics::arm_sve::svcntd()};

                if(element_count >= 8u)
                {
                    auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL8)};
                    auto vacc{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc)};
                    auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                                {
                                    auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                    auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                    auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                    auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                    auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                    auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                    auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                    acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                                }};
                    accrnd(vacc, 0u);
                    svst1_u64(mask, xacc, vacc);
                }
                else if(element_count == 2u)
                { /* sve128 */
                    auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL2)};
                    auto acc0{svld1_u64(mask, xacc + 0u)};
                    auto acc1{svld1_u64(mask, xacc + 2u)};
                    auto acc2{svld1_u64(mask, xacc + 4u)};
                    auto acc3{svld1_u64(mask, xacc + 6u)};
                    auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                                {
                                    auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                    auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                    auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                    auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                    auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                    auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                    auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                    acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                                }};
                    accrnd(acc0, 0u);
                    accrnd(acc1, 2u);
                    accrnd(acc2, 4u);
                    accrnd(acc3, 6u);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 0u, acc0);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 2u, acc1);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 4u, acc2);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 6u, acc3);
                }
                else
                {
                    auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL4)};
                    auto acc0{svld1_u64(mask, xacc + 0u)};
                    auto acc1{svld1_u64(mask, xacc + 4u)};
                    auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                                {
                                    auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                    auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                    auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                    auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                    auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                    auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                    auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                    acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                                }};
                    accrnd(acc0, 0u);
                    accrnd(acc1, 4u);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 0u, acc0);
                    ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 4u, acc1);
                }
            }
            ();

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__ARM_FEATURE_SVE)

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            auto xacc{reinterpret_cast<::std::uint64_t*>(acc_64aligned)};
            using uint64_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint64_t const*;
            auto xinput{reinterpret_cast<uint64_t_const_may_alias_ptr>(input)};
            auto xsecret{reinterpret_cast<uint64_t_const_may_alias_ptr>(secret)};
            auto const kSwap{::uwvm2::utils::intrinsics::arm_sve::sveor_n_u64_z(::uwvm2::utils::intrinsics::arm_sve::svptrue_b64(),
                                                                                ::uwvm2::utils::intrinsics::arm_sve::svindex_u64(0u, 1u),
                                                                                1u)};
            ::std::uint64_t element_count{::uwvm2::utils::intrinsics::arm_sve::svcntd()};

            if(element_count >= 8u)
            {
                auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL8)};
                auto vacc{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc)};
                auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                            {
                                auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                            }};
                accrnd(vacc, 0u);
                svst1_u64(mask, xacc, vacc);
            }
            else if(element_count == 2u)
            { /* sve128 */
                auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL2)};
                auto acc0{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 0u)};
                auto acc1{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 2u)};
                auto acc2{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 4u)};
                auto acc3{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 6u)};
                auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                            {
                                auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                            }};
                accrnd(acc0, 0u);
                accrnd(acc1, 2u);
                accrnd(acc2, 4u);
                accrnd(acc3, 6u);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 0u, acc0);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 2u, acc1);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 4u, acc2);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 6u, acc3);
            }
            else
            {
                auto mask{::uwvm2::utils::intrinsics::arm_sve::svptrue_pat_b64(::uwvm2::utils::intrinsics::arm_sve::SV_VL4)};
                auto acc0{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 0u)};
                auto acc1{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xacc + 4u)};
                auto accrnd{[&] UWVM_ALWAYS_INLINE(::uwvm2::utils::intrinsics::arm_sve::svuint64_t & acc, unsigned offset) constexpr noexcept -> void
                            {
                                auto const input_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xinput + offset)};
                                auto const secret_vec{::uwvm2::utils::intrinsics::arm_sve::svld1_u64(mask, xsecret + offset)};
                                auto const mixed{::uwvm2::utils::intrinsics::arm_sve::sveor_u64_x(mask, secret_vec, input_vec)};
                                auto const swapped{::uwvm2::utils::intrinsics::arm_sve::svtbl_u64(input_vec, kSwap)};
                                auto const mixed_lo{::uwvm2::utils::intrinsics::arm_sve::svextw_u64_x(mask, mixed)};
                                auto const mixed_hi{::uwvm2::utils::intrinsics::arm_sve::svlsr_n_u64_x(mask, mixed, 32u)};
                                auto const mul{::uwvm2::utils::intrinsics::arm_sve::svmad_u64_x(mask, mixed_lo, mixed_hi, swapped)};
                                acc = ::uwvm2::utils::intrinsics::arm_sve::svadd_u64_x(mask, acc, mul);
                            }};
                accrnd(acc0, 0u);
                accrnd(acc1, 4u);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 0u, acc0);
                ::uwvm2::utils::intrinsics::arm_sve::svst1_u64(mask, xacc + 4u, acc1);
            }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__ARM_NEON) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__loongarch_asx) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__loongarch_sx) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__wasm_simd128__) && 0
#else
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            using uint_least64_t_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint_least64_t*;
            auto xacc{reinterpret_cast<uint_least64_t_may_alias_ptr>(acc_64aligned)};

            for(unsigned i{}; i != 8u; ++i)
            {
                auto const data_val{xxh_readLE64(input)};
                input += 8uz;

                auto const data_key{data_val ^ xxh_readLE64(secret)};
                secret += 8uz;

                xacc[i ^ 1u] += data_val;
                if constexpr(dig64 != 64) { xacc[i ^ 1u] &= 0xFFFF'FFFF'FFFF'FFFFu; }

                xacc[i] = xxh_mult32to64_add64(data_key, data_key >> 32u, xacc[i]);
            }
#endif
        }

        inline constexpr void xxh3_scramble_acc(::std::byte* __restrict acc, ::std::byte const* __restrict secret) noexcept
        {
#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__AVX512F__) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__AVX2__) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__SSE2__) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                           \
    ((defined(UWVM_ENABLE_SME_SVE_STREAM_MODE) && defined(__ARM_FEATURE_SME)) && !defined(__ARM_FEATURE_SVE)) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__ARM_FEATURE_SVE) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__ARM_NEON) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__loongarch_asx) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__loongarch_sx) && 0
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && defined(__wasm_simd128__) && 0
#else
            constexpr auto dig64{::std::numeric_limits<::std::uint_least64_t>::digits};

            auto const acc_64aligned{::std::assume_aligned<64uz>(acc)};
            using uint_least64_t_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint_least64_t*;
            auto xacc{reinterpret_cast<uint_least64_t_may_alias_ptr>(acc_64aligned)};

            for(unsigned i{}; i != 8u; ++i)
            {
                auto const key64{xxh_readLE64(secret)};
                secret += 8uz;

                auto acc64{xacc[i]};
                acc64 = xxh_xorshift64(acc64, 47u);
                acc64 ^= key64;
                acc64 *= xxh_prime32_1;
                if constexpr(dig64 != 64) { acc64 &= 0xFFFF'FFFF'FFFF'FFFFu; }
                xacc[i] = acc64;
            }
#endif
        }

        inline constexpr void xxh3_accumulate(::std::uint_least64_t* __restrict acc,
                                              ::std::byte const* __restrict input,
                                              ::std::byte const* __restrict secret,
                                              ::std::size_t nbStripes) noexcept
        {
            for(::std::size_t n{}; n != nbStripes; ++n)
            {
                ::std::byte const* const in{input};
                input += 64u;
                ::uwvm2::utils::intrinsics::universal::prefetch<::uwvm2::utils::intrinsics::universal::pfc_mode::read,
                                                                ::uwvm2::utils::intrinsics::universal::pfc_level::L1,
                                                                ::uwvm2::utils::intrinsics::universal::ret_policy::keep>(
#ifdef __AVX512F__
                    in + 512u
#else
                    in + 384u
#endif
                );

                xxh3_accumulate_512(reinterpret_cast<::std::byte*>(acc), in, secret);
                secret += 8uz;
            }
        }

        inline constexpr ::std::uint_least64_t xxh3_hash_long_64bits_internal(::std::byte const* __restrict input,
                                                                              ::std::size_t len,
                                                                              ::std::byte const* __restrict secret,
                                                                              ::std::size_t secretSize) noexcept
        {
            alignas(64uz)::std::uint_least64_t
                acc[8uz]{xxh_prime32_3, xxh_prime64_1, xxh_prime64_2, xxh_prime64_3, xxh_prime64_4, xxh_prime32_2, xxh_prime64_5, xxh_prime32_1};
            static_assert(sizeof(acc) == 64uz);

            // acc can be aliased to other types for more aggressive optimization

            ::std::size_t const nbStripesPerBlock{(secretSize - 64uz) / 8uz};
            ::std::size_t const block_len{64uz * nbStripesPerBlock};
            ::std::size_t const nb_blocks{(len - 1uz) / block_len};

            auto input_curr{input};
            for(::std::size_t n{}; n != nb_blocks; ++n)
            {
                xxh3_accumulate(acc, input_curr, secret, nbStripesPerBlock);
                input_curr += block_len;
                xxh3_scramble_acc(reinterpret_cast<::std::byte*>(acc), secret + (secretSize - 64uz));
            }

            ::std::size_t const nbStripes{((len - 1uz) - (block_len * nb_blocks)) / 64uz};

            xxh3_accumulate(acc, input + nb_blocks * block_len, secret, nbStripes);

            /* last stripe */
            ::std::byte const* const p{input + (len - 64uz)};

            xxh3_accumulate_512(reinterpret_cast<::std::byte*>(acc), p, secret + (secretSize - 64uz - 7uz));

            return xxh3_finalize_long_64b(acc, secret, len);
        }

        inline constexpr ::std::uint_least64_t xxh3_64bits_internal(::std::byte const* __restrict input,
                                                                    ::std::size_t len,
                                                                    ::std::uint_least64_t seed64,
                                                                    ::std::byte const* __restrict secret,
                                                                    ::std::size_t secretLen) noexcept
        {
            constexpr ::std::size_t xxh3_secret_size_min{136uz};
            [[assume(secretLen >= xxh3_secret_size_min)]];

            /*
             * If an action is to be taken if `secretLen` condition is not respected,
             * it should be done here.
             * For now, it's a contract pre-condition.
             * Adding a check and a branch here would cost performance at every hash.
             * Also, note that function signature doesn't offer room to return an error.
             */
            if(len <= 16uz) { return xxh3_len_0to16_64b(input, len, secret, seed64); }
            else if(len <= 128uz) { return xxh3_len_17to128_64b(input, len, secret, secretLen, seed64); }
            else if(len <= 240uz) { return xxh3_len_129to240_64b(input, len, secret, secretLen, seed64); }
            else
            {
                return xxh3_hash_long_64bits_internal(input, len, secret, secretLen);
            }
        }

        template <xxh3_width_t Wid>
            requires (static_cast<unsigned>(Wid) <= static_cast<unsigned>(xxh3_width_t::XX3H_128))
        struct basic_xxh3_context
        {
            using digest_t = xxh3_output_struct<Wid>;

            inline static constexpr ::std::size_t digest_size{sizeof(digest_t)};

            inline constexpr void update([[maybe_unused]] ::std::byte const* first, [[maybe_unused]] ::std::byte const* last) noexcept {}

            inline constexpr void reset() noexcept {}

            inline constexpr digest_t digest_value() const noexcept {}

            inline constexpr void do_final() const noexcept {}

            inline constexpr void digest_to_byte_ptr([[maybe_unused]] ::std::byte* ptr) const noexcept {}
        };
    }  // namespace details

    using xxh3_64_context = details::basic_xxh3_context<details::xxh3_width_t::XX3H_64>;
    using xxh3_128_context = details::basic_xxh3_context<details::xxh3_width_t::XX3H_128>;
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
