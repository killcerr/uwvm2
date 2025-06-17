/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-31
 * @copyright   ASHP-1.0 License
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

#ifdef UWVM_MODULE
import fast_io;
import :base;
#else
// std
# include <cstdint>
# include <cstddef>
# include <climits>
# include <concepts>
# include <memory>
# include <bit>
// import
# include <fast_io.h>
# include "base.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::utf
{
    enum class utf8_specification : unsigned
    {
        utf8_rfc3629,
        utf8_rfc3629_and_zero_illegal
    };

    inline constexpr char32_t get_max_excessive_codepoint(utf8_specification u8spe) noexcept
    {
        switch(u8spe)
        {
            case utf8_specification::utf8_rfc3629:
            {
                [[fallthrough]];
            }
            case utf8_specification::utf8_rfc3629_and_zero_illegal:
            {
                return static_cast<char32_t>(0x10FFFFu);
            }
            [[unlikely]] default:
            {
                ::fast_io::fast_terminate();
            }
        }
    }

    template <bool zero_illegal>
    inline constexpr ::uwvm2::utils::utf::u8result check_legal_utf8_rfc3629_unchecked(char8_t const* const str_begin, char8_t const* const str_end) noexcept
    {
        constexpr unsigned char_bit{static_cast<unsigned>(CHAR_BIT)};
        constexpr bool char_bit_is_8{char_bit == 8u};
        constexpr bool is_little_endian{::std::endian::native == ::std::endian::little};

        if constexpr(is_little_endian && char_bit_is_8)
        {
            // scalar algorithm
            auto str_curr{str_begin};

            // Since there is no external macro protection, it is not possible to use (U)INTN_MAX, which the standard is not guaranteed to provide.
            // However, on platforms where CHAR_BIT is equal to 8, LEAST must have the same size as the N value.

#if SIZE_MAX > UINT_LEAST32_MAX
            auto str_curr{ str_begin };
            while (static_cast<::std::size_t>(str_end - str_curr) >= 4uz)
            {
                ::std::uint_least32_t need_check;

                ::fast_io::freestanding::my_memcpy(::std::addressof(need_check), str_curr, sizeof(::std::uint_least32_t));

                // It's already a little_endian.

                if (true)
                {
                    auto const ascii_size{ 0 };  // Get the number of preceding ascii characters

                    str_curr += ascii_size;

                    ::std::uint_least32_t const utf8{ static_cast<::std::uint_least32_t> (need_check) };

                    if ((utf8 & 0b11100000) == 0b11000000)
                    {
                        // Check if it is a utf8 sequence
                        auto utf8_mask_C0C0C000{ utf8 & static_cast<::std::uint_least32_t>(0x0000'C000u) };

                        auto constexpr utf8_need_chean{ 16u };

                        auto checker{ static_cast<::std::uint_least32_t>(0x0000'8000u) };

                        if (utf8_mask_C0C0C000 != checker) [[unlikely]] { return false; }

                        ::std::uint_least32_t utf8_clean{ utf8 };
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{ utf8_clean };
                        auto utf8_b0_shu1{ 8u - (2u + 1u) };
                        utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (2u - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{ utf8_clean };
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (2u - 2u) * 6u;

                        char32_t const utf8_c{ static_cast<char32_t>(utf8_b0 | utf8_b1) };

                        auto constexpr test_overlong{ 2u - 2u };
                        auto constexpr test_overlong_pc{ (test_overlong + 1u) >> 1u };

                        auto constexpr test_overlong_low_bound{ 1u << (6u * test_overlong + (8u - 2u + (1u - test_overlong_pc))) };
                        if (utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return false;
                        }

                        str_curr += 2u;
                        continue;
                    }
                    else if ((utf8 & 0b11110000) == 0b11100000)
                    {
                        // Check if it is a utf8 sequence
                        auto utf8_mask_C0C0C000{ utf8 & static_cast<::std::uint_least32_t>(0x00C0'C000u) };

                        auto constexpr utf8_need_chean{ 8u };

                        auto checker{ static_cast<::std::uint_least32_t>(0x0080'8000u) };

                        if (utf8_mask_C0C0C000 != checker) [[unlikely]] { return false; }

                        ::std::uint_least32_t utf8_clean{ utf8 };
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{ utf8_clean };
                        auto utf8_b0_shu1{ 8u - (3u + 1u) };
                        utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (3u - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{ utf8_clean };
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (3u - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{ utf8_clean };
                        utf8_b2 >>= 16u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (3u - 3u) * 6u;

                        char32_t const utf8_c{ static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2) };

                        auto constexpr test_overlong{ 3u - 2u };
                        auto constexpr test_overlong_pc{ (test_overlong + 1u) >> 1u };

                        auto constexpr test_overlong_low_bound{ 1u << (6u * test_overlong + (8u - 3u + (1u - test_overlong_pc))) };
                        if (utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return false;
                        }

                        if (static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                        {
                            return false;
                        }

                        str_curr += 3u;
                        continue;

                    }
                    else if ((utf8 & 0b11111000) == 0b11110000)
                    {
                        // Check if it is a utf8 sequence
                        auto utf8_mask_C0C0C000{ utf8 & static_cast<::std::uint_least32_t>(0xC0C0'C000u) };

                        auto constexpr utf8_need_chean{ 8u };

                        auto checker{ static_cast<::std::uint_least32_t>(0x8080'8000u) };

                        if (utf8_mask_C0C0C000 != checker) [[unlikely]] { return false; }

                        ::std::uint_least32_t utf8_clean{ utf8 };
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{ utf8_clean };
                        auto utf8_b0_shu1{ 8u - (4u + 1u) };
                        utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (4u - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{ utf8_clean };
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (4u - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{ utf8_clean };
                        utf8_b2 >>= 16u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (4u - 3u) * 6u;

                        ::std::uint_least32_t utf8_b3{ utf8_clean };
                        utf8_b2 >>= 24u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (4u - 4u) * 6u;

                        char32_t const utf8_c{ static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3) };

                        auto constexpr test_overlong{ 4u - 2u };
                        auto constexpr test_overlong_pc{ (test_overlong + 1u) >> 1u };

                        auto constexpr test_overlong_low_bound{ 1u << (6u * test_overlong + (8u - 4u + (1u - test_overlong_pc))) };
                        if (utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return false;
                        }

                        str_curr += 4u;
                        continue;

                    }
                    else
                    {
                        return false;
                    }

                }
                else
                {
                    // All legal ascii.
                    str_curr += 8u;
                }
            }
    return true;
        }
#endif

        // tail handling


    }

    template <utf8_specification spec>
        requires (static_cast<unsigned>(spec) <= static_cast<unsigned>(utf8_specification::utf8_rfc3629_and_zero_illegal))
    inline constexpr ::uwvm2::utils::utf::u8result check_legal_utf8_unchecked(char8_t const* const str_begin, char8_t const* const str_end) noexcept
    {
        if constexpr(spec == utf8_specification::utf8_rfc3629) { return check_legal_utf8_rfc3629_unchecked<false>(str_begin, str_end); }
        else if constexpr(spec == utf8_specification::utf8_rfc3629_and_zero_illegal) { return check_legal_utf8_rfc3629_unchecked<true>(str_begin, str_end); }
    }

    template <utf8_specification spec>
        requires (static_cast<unsigned>(spec) <= static_cast<unsigned>(utf8_specification::utf8_rfc3629_and_zero_illegal))
    inline constexpr ::uwvm2::utils::utf::u8result check_legal_utf8(char8_t const* const str_begin, char8_t const* const str_end) noexcept
    {
        if(str_begin > str_end) [[unlikely]] { ::fast_io::fast_terminate(); }

        return check_legal_utf8_unchecked<spec>(str_begin, str_end);
    }

}  // namespace uwvm2::utils::utf
