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
        rfc3629,
        rfc3629_and_zero_illegal
    };

    inline constexpr char32_t get_max_excessive_codepoint(utf8_specification u8spe) noexcept
    {
        switch(u8spe)
        {
            case utf8_specification::rfc3629:
            {
                [[fallthrough]];
            }
            case utf8_specification::rfc3629_and_zero_illegal:
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
            while(static_cast<::std::size_t>(str_end - str_curr) >= 8uz)
            {
                ::std::uint_least64_t need_check;

                ::fast_io::freestanding::my_memcpy(::std::addressof(need_check), str_curr, sizeof(::std::uint_least64_t));

                // It's already a little_endian.

                auto const has_utf8_mask{need_check & static_cast<::std::uint_least64_t>(0x8080'8080'8080'8080u)};

                if(has_utf8_mask)
                {
                    auto const rz{static_cast<unsigned>(::std::countr_zero(has_utf8_mask))};
                    auto const ascii_size{rz / 8u};  // Get the number of preceding ascii characters
                    auto const utf8_length{static_cast<unsigned>(::std::countl_one(need_check << (64u - (ascii_size + 1u) * 8u)))};  // Get the length of utf8

                    if(utf8_length > 4u) [[unlikely]] { return {::uwvm2::utils::utf::utf_error_code::long_header_bits, str_curr}; }
                    else if(utf8_length == 1u) [[unlikely]] { return {::uwvm2::utils::utf::utf_error_code::too_long_sequence, str_curr}; }

# if __has_cpp_attribute(assume)
                    [[assume(utf8_length != 0u)]];
# endif

                    if(ascii_size + utf8_length > 8u)
                    {
                        // The round of calculations that can't be accommodated under a reread.
                        str_curr += ascii_size;

                        if(static_cast<::std::size_t>(str_end - str_curr) < 4uz) [[unlikely]] { break; }

                        ::std::uint_least32_t utf8;
                        ::fast_io::freestanding::my_memcpy(::std::addressof(utf8), str_curr, sizeof(::std::uint_least32_t));

                        // Check if it is a utf8 sequence
                        auto utf8_mask_C0C0C000{utf8 & static_cast<::std::uint_least32_t>(0xC0C0'C000u)};

                        auto const utf8_need_chean{(4u - utf8_length) * 8u};
                        utf8_mask_C0C0C000 <<= utf8_need_chean;
                        utf8_mask_C0C0C000 >>= utf8_need_chean;

                        auto checker{static_cast<::std::uint_least32_t>(0x8080'8000u)};
                        checker <<= utf8_need_chean;
                        checker >>= utf8_need_chean;

                        if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {::uwvm2::utils::utf::utf_error_code::too_short_sequence, str_curr}; }

                        ::std::uint_least32_t utf8_clean{utf8};
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{utf8_clean};
                        auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{utf8_clean};
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{utf8_clean};
                        utf8_b2 >>= 16u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (utf8_length - 3u) * 6u;

                        ::std::uint_least32_t utf8_b3{utf8_clean};
                        utf8_b3 >>= 24u;
                        utf8_b3 &= 0x3Fu;
                        utf8_b3 <<= (utf8_length - 4u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3)};

                        if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::excessive_codepoint, str_curr};
                        }

                        auto const test_overlong{utf8_length - 2u};
                        auto const test_overlong_pc{(test_overlong + 1u) >> 1u};

                        auto const test_overlong_low_bound{1u << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::overlong_encoding, str_curr};
                        }

                        if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::illegal_surrogate, str_curr};
                        }

                        str_curr += utf8_length;
                    }
                    else
                    {
                        str_curr += ascii_size;

                        ::std::uint_least32_t const utf8{static_cast<::std::uint_least32_t>(need_check >> (ascii_size * 8u))};

                        // Check if it is a utf8 sequence
                        auto utf8_mask_C0C0C000{utf8 & static_cast<::std::uint_least32_t>(0xC0C0'C000u)};

                        auto const utf8_need_chean{(4u - utf8_length) * 8u};
                        utf8_mask_C0C0C000 <<= utf8_need_chean;
                        utf8_mask_C0C0C000 >>= utf8_need_chean;

                        auto checker{static_cast<::std::uint_least32_t>(0x8080'8000u)};
                        checker <<= utf8_need_chean;
                        checker >>= utf8_need_chean;

                        if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {::uwvm2::utils::utf::utf_error_code::too_short_sequence, str_curr}; }

                        ::std::uint_least32_t utf8_clean{utf8};
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{utf8_clean};
                        auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{utf8_clean};
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{utf8_clean};
                        utf8_b2 >>= 16u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (utf8_length - 3u) * 6u;

                        ::std::uint_least32_t utf8_b3{utf8_clean};
                        utf8_b3 >>= 24u;
                        utf8_b3 &= 0x3Fu;
                        utf8_b3 <<= (utf8_length - 4u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3)};

                        if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::excessive_codepoint, str_curr};
                        }

                        auto const test_overlong{utf8_length - 2u};
                        auto const test_overlong_pc{(test_overlong + 1u) >> 1u};

                        auto const test_overlong_low_bound{1u << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::overlong_encoding, str_curr};
                        }

                        if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                        {
                            return {::uwvm2::utils::utf::utf_error_code::illegal_surrogate, str_curr};
                        }

                        str_curr += utf8_length;
                    }
                }
                else
                {
                    // All legal ascii.
                    str_curr += 8u;
                }
            }

#endif

            // tail handler
#if SIZE_MAX > UINT_LEAST32_MAX

#else

#endif
        }
        else
        {
            // big_endian. pdp_endian and CHAR_BIT != 8
        }
    }

    template <utf8_specification spec>
        requires (static_cast<unsigned>(spec) <= static_cast<unsigned>(utf8_specification::rfc3629_and_zero_illegal))
    inline constexpr ::uwvm2::utils::utf::u8result check_legal_utf8_unchecked(char8_t const* const str_begin, char8_t const* const str_end) noexcept
    {
        if constexpr(spec == utf8_specification::rfc3629) { return check_legal_utf8_rfc3629_unchecked<false>(str_begin, str_end); }
        else if constexpr(spec == utf8_specification::rfc3629_and_zero_illegal) { return check_legal_utf8_rfc3629_unchecked<true>(str_begin, str_end); }
    }

    template <utf8_specification spec>
        requires (static_cast<unsigned>(spec) <= static_cast<unsigned>(utf8_specification::rfc3629_and_zero_illegal))
    inline constexpr ::uwvm2::utils::utf::u8result check_legal_utf8(char8_t const* const str_begin, char8_t const* const str_end) noexcept
    {
        if(str_begin > str_end) [[unlikely]] { ::fast_io::fast_terminate(); }

        return check_legal_utf8_unchecked<spec>(str_begin, str_end);
    }

}  // namespace uwvm2::utils::utf
