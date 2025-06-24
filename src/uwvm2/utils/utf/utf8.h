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
# include <limits>
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
        // Since most of the import/export and wasi used within wasm are ascii, the utf8 path is less efficient.
        // sse4, avx2, avx512vbmi, neon, lsx, lasx using the same algorithm as simdutf, see https://github.com/simdutf/simdutf

        auto str_curr{str_begin};

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX512VBMI__) || defined(__AVX512VBMI2__)) & 0

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX2__) || defined(__loongarch_asx)) & 0

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__SSSE3__) || defined(__ARM_NEON) || defined(__loongarch_sx)) & 0

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__SSE2__) || defined(__wasm_simd128__))

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        for(;;)
        {
            if(static_cast<::std::size_t>(str_end - str_curr) >= sizeof(u8x16simd)) [[likely]]
            {
                u8x16simd need_check;

                ::fast_io::freestanding::my_memcpy(::std::addressof(need_check), str_curr, sizeof(u8x16simd));

                bool const check_has_non_ascii{static_cast<bool>(
# if defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(need_check))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~need_check))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(need_check))
# else
#  error "missing instructions"
# endif
                        )};

                if(!check_has_non_ascii)
                {
                    // all ascii

                    if constexpr(zero_illegal)
                    {
                        constexpr auto u8_zero{static_cast<::std::uint8_t>(0u)};

                        auto const has_zero{need_check == u8_zero};

                        bool const check_has_zero{static_cast<bool>(
# if defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                            __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(has_zero))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                            !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~has_zero))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                            __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(has_zero))
# else
#  error "missing instructions"
# endif
                                )};

                        if(check_has_zero) [[unlikely]]
                        {
                            // Get the exact error address
                            for(auto const str_curr_end{str_curr + sizeof(u8x16simd)}; str_curr != str_curr_end; ++str_curr)
                            {
                                if(*str_curr == u8'\0') { break; }
                            }

                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters};
                        }
                    }

                    str_curr += sizeof(u8x16simd);
                    continue;
                }
            }

            while(*str_curr < static_cast<char8_t>(0b1000'0000u))
            {
                if constexpr(zero_illegal)
                {
                    if(*str_curr == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                }

                if(++str_curr == str_end) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::success}; }
            }

            if(static_cast<::std::size_t>(str_end - str_curr) >= sizeof(::std::uint_least32_t)) [[likely]]
            {
                ::std::uint_least32_t utf_8;

                ::fast_io::freestanding::my_memcpy(::std::addressof(utf_8), str_curr, sizeof(::std::uint_least32_t));

                if((utf_8 & static_cast<::std::uint_least32_t>(0b1110'0000u)) == static_cast<::std::uint_least32_t>(0b1100'0000u))
                {
                    // Check if it is a utf_8 sequence
                    auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0x0000'C000u)};

                    constexpr auto utf8_need_chean{16u};
                    constexpr auto utf8_length{2u};

                    constexpr auto checker{static_cast<::std::uint_least32_t>(0x0000'8000u)};

                    if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                    ::std::uint_least32_t utf8_clean{utf_8};
                    // Automatic optimization into `and`
                    utf8_clean <<= utf8_need_chean;
                    utf8_clean >>= utf8_need_chean;

                    ::std::uint_least32_t utf8_b0{utf8_clean};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                    utf8_b0 <<= (utf8_length - 1u) * 6u;

                    ::std::uint_least32_t utf8_b1{utf8_clean};
                    utf8_b1 >>= 8u;
                    utf8_b1 &= 0x3Fu;
                    utf8_b1 <<= (utf8_length - 2u) * 6u;

                    char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    str_curr += utf8_length;
                }
                else if((utf_8 & static_cast<::std::uint_least32_t>(0b1111'0000u)) == static_cast<::std::uint_least32_t>(0b1110'0000u))
                {
                    // Check if it is a utf_8 sequence
                    auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0x00C0'C000u)};

                    constexpr auto utf8_need_chean{8u};
                    constexpr auto utf8_length{3u};

                    constexpr auto checker{static_cast<::std::uint_least32_t>(0x0080'8000u)};

                    if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                    ::std::uint_least32_t utf8_clean{utf_8};
                    // Automatic optimization into `and`
                    utf8_clean <<= utf8_need_chean;
                    utf8_clean >>= utf8_need_chean;

                    ::std::uint_least32_t utf8_b0{utf8_clean};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                    utf8_b0 <<= (utf8_length - 1u) * 6u;

                    ::std::uint_least32_t utf8_b1{utf8_clean};
                    utf8_b1 >>= 8u;
                    utf8_b1 &= 0x3Fu;
                    utf8_b1 <<= (utf8_length - 2u) * 6u;

                    ::std::uint_least32_t utf8_b2{utf8_clean};
                    utf8_b2 >>= 16u;
                    utf8_b2 &= 0x3Fu;
                    utf8_b2 <<= (utf8_length - 3u) * 6u;

                    char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2)};

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::illegal_surrogate};
                    }

                    str_curr += utf8_length;
                }
                else if((utf_8 & static_cast<::std::uint_least32_t>(0b1111'1000u)) == static_cast<::std::uint_least32_t>(0b1111'0000u))
                {
                    // Check if it is a utf_8 sequence
                    auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0xC0C0'C000u)};

                    constexpr auto utf8_length{4u};

                    constexpr auto checker{static_cast<::std::uint_least32_t>(0x8080'8000u)};

                    if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                    ::std::uint_least32_t utf8_clean{utf_8};  // utf8_need_chean == 0u

                    ::std::uint_least32_t utf8_b0{utf8_clean};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
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

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::excessive_codepoint}; }

                    str_curr += utf8_length;
                }
                else [[unlikely]]
                {
                    if((utf_8 & static_cast<::std::uint_least32_t>(0b1100'0000u)) == static_cast<::std::uint_least32_t>(0b1000'0000u))
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_long_sequence};
                    }
                    else
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
                    }
                }

                // continue;
            }
            else
            {
                // byte-by-byte processing

                while(str_curr != str_end)
                {
                    auto str_curr_val{*str_curr};

                    if(str_curr_val < static_cast<char8_t>(0b1000'0000u))
                    {
                        if constexpr(zero_illegal)
                        {
                            if(str_curr_val == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                        }

                        ++str_curr;
                    }
                    else if((str_curr_val & static_cast<char8_t>(0b1110'0000u)) == static_cast<char8_t>(0b1100'0000u))
                    {
                        if(static_cast<::std::size_t>(str_end - str_curr) < 2uz) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p1_val{*(str_curr + 1u)};

                        if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        constexpr auto utf8_length{2u};

                        ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        str_curr += utf8_length;
                    }
                    else if((str_curr_val & static_cast<char8_t>(0b1111'0000u)) == static_cast<char8_t>(0b1110'0000u))
                    {
                        if(static_cast<::std::size_t>(str_end - str_curr) < 3uz) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p1_val{*(str_curr + 1u)};

                        if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p2_val{*(str_curr + 2u)};

                        if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        constexpr auto utf8_length{3u};

                        ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (utf8_length - 3u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2)};

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::illegal_surrogate};
                        }

                        str_curr += utf8_length;
                    }
                    else if((str_curr_val & static_cast<char8_t>(0b1111'1000u)) == static_cast<char8_t>(0b1111'0000u))
                    {
                        if(static_cast<::std::size_t>(str_end - str_curr) < 4uz) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p1_val{*(str_curr + 1u)};

                        if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p2_val{*(str_curr + 2u)};

                        if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        auto const str_curr_p3_val{*(str_curr + 3u)};

                        if((str_curr_p3_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                        }

                        constexpr auto utf8_length{4u};

                        ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (utf8_length - 3u) * 6u;

                        ::std::uint_least32_t utf8_b3{static_cast<::std::uint_least32_t>(str_curr_p3_val)};
                        utf8_b3 &= 0x3Fu;
                        utf8_b3 <<= (utf8_length - 4u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3)};

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::excessive_codepoint};
                        }

                        str_curr += utf8_length;
                    }
                    else
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
                    }
                }

                // jump out of a loop
                break;
            }
        }

#else

        constexpr unsigned char_bit{static_cast<unsigned>(CHAR_BIT)};
        constexpr bool char_bit_is_8{char_bit == 8u};
        constexpr bool is_little_endian{::std::endian::native == ::std::endian::little};

        if constexpr(is_little_endian && char_bit_is_8 && ::std::same_as<::std::size_t, ::std::uint_least64_t> &&
                     static_cast<unsigned>(::std::numeric_limits<::std::uint_least64_t>::digits) == 64u)
        {
            // SWAR algorithm

            // Avoid using (u)intN_t in non-macro-qualified environments, as it may not be provided, use (u)int_leastN_t and determine its digits to determine
            // if it is the same type as the expected (u)intN_t

            // Since there is no external macro protection, it is not possible to use (U)INTN_MAX, which the standard is not guaranteed to provide.
            // However, on platforms where CHAR_BIT is equal to 8, LEAST must have the same size as the N value.

            for(;;)
            {
                if(static_cast<::std::size_t>(str_end - str_curr) >= 2uz * sizeof(::std::uint_least64_t)) [[likely]]
                {
                    ::std::uint_least64_t need_check1;

                    ::fast_io::freestanding::my_memcpy(::std::addressof(need_check1), str_curr, sizeof(::std::uint_least64_t));

                    ::std::uint_least64_t need_check2;

                    ::fast_io::freestanding::my_memcpy(::std::addressof(need_check2), str_curr + sizeof(::std::uint_least64_t), sizeof(::std::uint_least64_t));

                    ::std::uint_least64_t const check_has_non_ascii{(need_check1 | need_check2) & static_cast<::std::uint_least64_t>(0x8080'8080'8080'8080u)};

                    if(!check_has_non_ascii)
                    {
                        if constexpr(zero_illegal)
                        {
                            if(~((((need_check1 & static_cast<::std::uint_least64_t>(0x7F7F'7F7F'7F7F'7F7Fu)) + 0x7F7F'7F7F'7F7F'7F7Fu) | need_check1) |
                                 0x7F7F'7F7F'7F7F'7F7Fu)) [[unlikely]]
                            {
                                // Get the exact error address
                                for(auto const str_curr_end{str_curr + sizeof(::std::uint_least64_t)}; str_curr != str_curr_end; ++str_curr)
                                {
                                    if(*str_curr == u8'\0') { break; }
                                }

                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters};
                            }

                            if(~((((need_check2 & static_cast<::std::uint_least64_t>(0x7F7F'7F7F'7F7F'7F7Fu)) + 0x7F7F'7F7F'7F7F'7F7Fu) | need_check2) |
                                 0x7F7F'7F7F'7F7F'7F7Fu)) [[unlikely]]
                            {
                                // Get the exact error address
                                str_curr += sizeof(::std::uint_least64_t);

                                for(auto const str_curr_end{str_curr + sizeof(::std::uint_least64_t)}; str_curr != str_curr_end; ++str_curr)
                                {
                                    if(*str_curr == u8'\0') { break; }
                                }

                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters};
                            }
                        }

                        str_curr += 2u * sizeof(::std::uint_least64_t);
                        continue;
                    }
                }

                while(*str_curr < static_cast<char8_t>(0b1000'0000u))
                {
                    if constexpr(zero_illegal)
                    {
                        if(*str_curr == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                    }

                    if(++str_curr == str_end) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::success}; }
                }

                if(static_cast<::std::size_t>(str_end - str_curr) >= sizeof(::std::uint_least32_t)) [[likely]]
                {
                    ::std::uint_least32_t utf_8;

                    ::fast_io::freestanding::my_memcpy(::std::addressof(utf_8), str_curr, sizeof(::std::uint_least32_t));

                    if((utf_8 & static_cast<::std::uint_least32_t>(0b1110'0000u)) == static_cast<::std::uint_least32_t>(0b1100'0000u))
                    {
                        // Check if it is a utf_8 sequence
                        auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0x0000'C000u)};

                        constexpr auto utf8_need_chean{16u};
                        constexpr auto utf8_length{2u};

                        constexpr auto checker{static_cast<::std::uint_least32_t>(0x0000'8000u)};

                        if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                        ::std::uint_least32_t utf8_clean{utf_8};
                        // Automatic optimization into `and`
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{utf8_clean};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{utf8_clean};
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        str_curr += utf8_length;
                    }
                    else if((utf_8 & static_cast<::std::uint_least32_t>(0b1111'0000u)) == static_cast<::std::uint_least32_t>(0b1110'0000u))
                    {
                        // Check if it is a utf_8 sequence
                        auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0x00C0'C000u)};

                        constexpr auto utf8_need_chean{8u};
                        constexpr auto utf8_length{3u};

                        constexpr auto checker{static_cast<::std::uint_least32_t>(0x0080'8000u)};

                        if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                        ::std::uint_least32_t utf8_clean{utf_8};
                        // Automatic optimization into `and`
                        utf8_clean <<= utf8_need_chean;
                        utf8_clean >>= utf8_need_chean;

                        ::std::uint_least32_t utf8_b0{utf8_clean};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                        utf8_b0 <<= (utf8_length - 1u) * 6u;

                        ::std::uint_least32_t utf8_b1{utf8_clean};
                        utf8_b1 >>= 8u;
                        utf8_b1 &= 0x3Fu;
                        utf8_b1 <<= (utf8_length - 2u) * 6u;

                        ::std::uint_least32_t utf8_b2{utf8_clean};
                        utf8_b2 >>= 16u;
                        utf8_b2 &= 0x3Fu;
                        utf8_b2 <<= (utf8_length - 3u) * 6u;

                        char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2)};

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::illegal_surrogate};
                        }

                        str_curr += utf8_length;
                    }
                    else if((utf_8 & static_cast<::std::uint_least32_t>(0b1111'1000u)) == static_cast<::std::uint_least32_t>(0b1111'0000u))
                    {
                        // Check if it is a utf_8 sequence
                        auto utf8_mask_C0C0C000{utf_8 & static_cast<::std::uint_least32_t>(0xC0C0'C000u)};

                        constexpr auto utf8_length{4u};

                        constexpr auto checker{static_cast<::std::uint_least32_t>(0x8080'8000u)};

                        if(utf8_mask_C0C0C000 != checker) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence}; }

                        ::std::uint_least32_t utf8_clean{utf_8};  // utf8_need_chean == 0u

                        ::std::uint_least32_t utf8_b0{utf8_clean};
                        constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                        utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
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

                        constexpr auto test_overlong{utf8_length - 2u};
                        constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                        if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                        }

                        if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]]
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::excessive_codepoint};
                        }

                        str_curr += utf8_length;
                    }
                    else [[unlikely]]
                    {
                        if((utf_8 & static_cast<::std::uint_least32_t>(0b1100'0000u)) == static_cast<::std::uint_least32_t>(0b1000'0000u))
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_long_sequence};
                        }
                        else
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
                        }
                    }

                    // continue;
                }
                else
                {
                    // byte-by-byte processing

                    while(str_curr != str_end)
                    {
                        auto const str_curr_val{*str_curr};

                        if(str_curr_val < static_cast<char8_t>(0b1000'0000u))
                        {
                            if constexpr(zero_illegal)
                            {
                                if(str_curr_val == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                            }

                            ++str_curr;
                        }
                        else if((str_curr_val & static_cast<char8_t>(0b1110'0000u)) == static_cast<char8_t>(0b1100'0000u))
                        {
                            if(static_cast<::std::size_t>(str_end - str_curr) < 2uz) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p1_val{*(str_curr + 1u)};

                            if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            constexpr auto utf8_length{2u};

                            ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                            constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                            utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                            utf8_b0 <<= (utf8_length - 1u) * 6u;

                            ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                            utf8_b1 &= 0x3Fu;
                            utf8_b1 <<= (utf8_length - 2u) * 6u;

                            char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                            constexpr auto test_overlong{utf8_length - 2u};
                            constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                            if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                            }

                            str_curr += utf8_length;
                        }
                        else if((str_curr_val & static_cast<char8_t>(0b1111'0000u)) == static_cast<char8_t>(0b1110'0000u))
                        {
                            if(static_cast<::std::size_t>(str_end - str_curr) < 3uz) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p1_val{*(str_curr + 1u)};

                            if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p2_val{*(str_curr + 2u)};

                            if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            constexpr auto utf8_length{3u};

                            ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                            constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                            utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                            utf8_b0 <<= (utf8_length - 1u) * 6u;

                            ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                            utf8_b1 &= 0x3Fu;
                            utf8_b1 <<= (utf8_length - 2u) * 6u;

                            ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                            utf8_b2 &= 0x3Fu;
                            utf8_b2 <<= (utf8_length - 3u) * 6u;

                            char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2)};

                            constexpr auto test_overlong{utf8_length - 2u};
                            constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                            if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                            }

                            if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::illegal_surrogate};
                            }

                            str_curr += utf8_length;
                        }
                        else if((str_curr_val & static_cast<char8_t>(0b1111'1000u)) == static_cast<char8_t>(0b1111'0000u))
                        {
                            if(static_cast<::std::size_t>(str_end - str_curr) < 4uz) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p1_val{*(str_curr + 1u)};

                            if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p2_val{*(str_curr + 2u)};

                            if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            auto const str_curr_p3_val{*(str_curr + 3u)};

                            if((str_curr_p3_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                            }

                            constexpr auto utf8_length{4u};

                            ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                            constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                            utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                            utf8_b0 <<= (utf8_length - 1u) * 6u;

                            ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                            utf8_b1 &= 0x3Fu;
                            utf8_b1 <<= (utf8_length - 2u) * 6u;

                            ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                            utf8_b2 &= 0x3Fu;
                            utf8_b2 <<= (utf8_length - 3u) * 6u;

                            ::std::uint_least32_t utf8_b3{static_cast<::std::uint_least32_t>(str_curr_p3_val)};
                            utf8_b3 &= 0x3Fu;
                            utf8_b3 <<= (utf8_length - 4u) * 6u;

                            char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3)};

                            constexpr auto test_overlong{utf8_length - 2u};
                            constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                            if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                            }

                            if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]]
                            {
                                return {str_curr, ::uwvm2::utils::utf::utf_error_code::excessive_codepoint};
                            }

                            str_curr += utf8_length;
                        }
                        else
                        {
                            return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
                        }
                    }

                    // jump out of a loop
                    break;
                }
            }
        }
        else
        {
            // byte-by-byte processing

            while(str_curr != str_end)
            {
                // [xx] xx xx ... (end)
                // [  ] unsafe (could be the section_end)
                //  ^^ section_curr

                auto const str_curr_val{*str_curr};

                if(str_curr_val < static_cast<char8_t>(0b1000'0000u))
                {
                    if constexpr(zero_illegal)
                    {
                        if(str_curr_val == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                    }

                    ++str_curr;
                }
                else if((str_curr_val & static_cast<char8_t>(0b1110'0000u)) == static_cast<char8_t>(0b1100'0000u))
                {
                    if(static_cast<::std::size_t>(str_end - str_curr) < 2uz) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p1_val{*(str_curr + 1u)};

                    if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    constexpr auto utf8_length{2u};

                    ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                    utf8_b0 <<= (utf8_length - 1u) * 6u;

                    ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                    utf8_b1 &= 0x3Fu;
                    utf8_b1 <<= (utf8_length - 2u) * 6u;

                    char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    str_curr += utf8_length;
                }
                else if((str_curr_val & static_cast<char8_t>(0b1111'0000u)) == static_cast<char8_t>(0b1110'0000u))
                {
                    if(static_cast<::std::size_t>(str_end - str_curr) < 3uz) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p1_val{*(str_curr + 1u)};

                    if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p2_val{*(str_curr + 2u)};

                    if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    constexpr auto utf8_length{3u};

                    ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                    utf8_b0 <<= (utf8_length - 1u) * 6u;

                    ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                    utf8_b1 &= 0x3Fu;
                    utf8_b1 <<= (utf8_length - 2u) * 6u;

                    ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                    utf8_b2 &= 0x3Fu;
                    utf8_b2 <<= (utf8_length - 3u) * 6u;

                    char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2)};

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    if(static_cast<char32_t>(0xD7FFu) < utf8_c && utf8_c < static_cast<char32_t>(0xE000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::illegal_surrogate};
                    }

                    str_curr += utf8_length;
                }
                else if((str_curr_val & static_cast<char8_t>(0b1111'1000u)) == static_cast<char8_t>(0b1111'0000u))
                {
                    if(static_cast<::std::size_t>(str_end - str_curr) < 4uz) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p1_val{*(str_curr + 1u)};

                    if((str_curr_p1_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p2_val{*(str_curr + 2u)};

                    if((str_curr_p2_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    auto const str_curr_p3_val{*(str_curr + 3u)};

                    if((str_curr_p3_val & static_cast<char8_t>(0b1100'0000u)) != static_cast<char8_t>(0b1000'0000u)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::too_short_sequence};
                    }

                    constexpr auto utf8_length{4u};

                    ::std::uint_least32_t utf8_b0{static_cast<::std::uint_least32_t>(str_curr_val)};
                    constexpr auto utf8_b0_shu1{8u - (utf8_length + 1u)};
                    utf8_b0 &= ((static_cast<::std::uint_least32_t>(1u) << utf8_b0_shu1) - 1u);
                    utf8_b0 <<= (utf8_length - 1u) * 6u;

                    ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                    utf8_b1 &= 0x3Fu;
                    utf8_b1 <<= (utf8_length - 2u) * 6u;

                    ::std::uint_least32_t utf8_b2{static_cast<::std::uint_least32_t>(str_curr_p2_val)};
                    utf8_b2 &= 0x3Fu;
                    utf8_b2 <<= (utf8_length - 3u) * 6u;

                    ::std::uint_least32_t utf8_b3{static_cast<::std::uint_least32_t>(str_curr_p3_val)};
                    utf8_b3 &= 0x3Fu;
                    utf8_b3 <<= (utf8_length - 4u) * 6u;

                    char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1 | utf8_b2 | utf8_b3)};

                    constexpr auto test_overlong{utf8_length - 2u};
                    constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u) << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    if(utf8_c > static_cast<char32_t>(0x10FFFFu)) [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::excessive_codepoint}; }

                    str_curr += utf8_length;
                }
                else
                {
                    return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
                }
            }
        }
#endif

        return {str_curr, ::uwvm2::utils::utf::utf_error_code::success};
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
