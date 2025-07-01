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
import uwvm2.utils.intrinsics;
import :base;
#else
// std
# include <cstdint>
# include <cstddef>
# include <climits>
# include <cstring>
# include <concepts>
# include <memory>
# include <bit>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/intrinsics/impl.h>
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

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX512VBMI__) || defined(__AVX512VBMI2__)) && 0

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX2__) || defined(__loongarch_asx)) && 0

#elif __has_cpp_attribute(__gnu__::__vector_size__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) && defined(__LITTLE_ENDIAN__) &&                              \
    (defined(__SSSE3__) || defined(__ARM_NEON) || defined(__loongarch_sx))

        // Same algorithm as simdutf

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x8simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        constexpr auto check_special_cases{
            [] UWVM_ALWAYS_INLINE(u8x16simd const input, u8x16simd const prev1) constexpr noexcept -> u8x16simd
            {
                constexpr ::std::uint8_t too_short{1u << 0u};
                // 11??'???? 0???'????
                // 11??'???? 11??'????
                constexpr ::std::uint8_t too_long{1u << 1u};
                // 0???'???? 10??'????
                constexpr ::std::uint8_t overlong_3{1u << 2u};
                // 1110'0000 100?'????
                constexpr ::std::uint8_t surrogate{1u << 4u};
                // 1110'1101 101?'????
                constexpr ::std::uint8_t overlong_2{1u << 5u};
                // 1100'000? 10??'????
                constexpr ::std::uint8_t two_conts{1u << 7u};
                // 10??'???? 10??'????
                constexpr ::std::uint8_t too_large{1u << 3u};
                // 1111'0100 1001'????
                // 1111'0100 101?'????
                // 1111'0101 1001'????
                // 1111'0101 101?'????
                // 1111'011? 1001'????
                // 1111'011? 101?'????
                // 1111'1??? 1001'????
                // 1111'1??? 101?'????
                constexpr ::std::uint8_t too_large_1000{1u << 6u};
                // 1111'0101 1000'????
                // 1111'011? 1000'????
                // 1111'1??? 1000'????
                constexpr ::std::uint8_t overlong_4{1u << 6u};
                // 1111'0000 1000'????
                [[maybe_unused]] constexpr ::std::uint8_t forbidden{0xFFu};

                constexpr u8x16simd lookup_b1h{// 0???'???? ????'???? <ASCII in byte 1>
                                               too_long,
                                               too_long,
                                               too_long,
                                               too_long,
                                               too_long,
                                               too_long,
                                               too_long,
                                               too_long,
                                               // 10??'???? ????'???? <continuation in byte 1>
                                               two_conts,
                                               two_conts,
                                               two_conts,
                                               two_conts,
                                               // 1100'???? ????'???? <two byte lead in byte 1>
                                               too_short | overlong_2,
                                               // 1101'???? ????'???? <two byte lead in byte 1>
                                               too_short,
                                               // 1110'???? ????'???? <three byte lead in byte 1>
                                               too_short | overlong_3 | surrogate,
                                               // 1111'???? ????'???? <four+ byte lead in byte 1>
                                               too_short | too_large | too_large_1000 | overlong_4};

                // These all have ???? in byte 1.
                constexpr ::std::uint8_t carry{too_short | too_long | two_conts};

                constexpr u8x16simd lookup_b1l{// ????'0000 ????'????
                                               carry | overlong_3 | overlong_2 | overlong_4,
                                               // ????'0001 ????'????
                                               carry | overlong_2,
                                               // ????'001? ????'????
                                               carry,
                                               carry,
                                               // ????'0100 ????'????
                                               carry | too_large,
                                               // ????'0101 ????'????
                                               carry | too_large | too_large_1000,
                                               // ????'011? ????'????
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000,
                                               // ????'1??? ????'????
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000,
                                               // ????'1101 ????'????
                                               carry | too_large | too_large_1000 | surrogate,
                                               carry | too_large | too_large_1000,
                                               carry | too_large | too_large_1000};

                constexpr u8x16simd lookup_b2h{// ????'???? 0???'????
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short,
                                               // ????'???? 1000'????
                                               too_long | overlong_2 | two_conts | overlong_3 | too_large_1000 | overlong_4,
                                               // ????'???? 1001'????
                                               too_long | overlong_2 | two_conts | overlong_3 | too_large,
                                               // ????'???? 101?'????
                                               too_long | overlong_2 | two_conts | surrogate | too_large,
                                               too_long | overlong_2 | two_conts | surrogate | too_large,
                                               // ????'???? 11??'????
                                               too_short,
                                               too_short,
                                               too_short,
                                               too_short};

                auto const prev1_shr4{::std::bit_cast<u8x16simd>(::std::bit_cast<u16x8simd>(prev1) >> static_cast<::std::uint16_t>(4u)) &
                                      static_cast<::std::uint8_t>(0xFFu >> 4u)};

                u8x16simd b1h;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                b1h = ::std::bit_cast<u8x16simd>(__builtin_ia32_pshufb128(lookup_b1h, prev1_shr4));
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_qtbl1v16qi_uuu)  // GCC
                b1h = __builtin_aarch64_qtbl1v16qi_uuu(prev1_shr4, lookup_b1h);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqtbl1q_v)          // Clang
                b1h = ::std::bit_cast<u8x16simd>(__builtin_neon_vqtbl1q_v(::std::bit_cast<i8x16simd>(prev1_shr4), ::std::bit_cast<i8x16simd>(lookup_b1h)));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                b1h = ::std::bit_cast<u8x16simd>(__builtin_lsx_vshuf_b(lookup_b1h, lookup_b1h, prev1_shr4));
# else
#  error "missing instructions"
# endif

                const auto prev1_andF{prev1 & static_cast<::std::uint8_t>(0xFu)};

                u8x16simd b1l;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                b1l = ::std::bit_cast<u8x16simd>(__builtin_ia32_pshufb128(lookup_b1l, prev1_andF));
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_qtbl1v16qi_uuu)  // GCC
                b1l = __builtin_aarch64_qtbl1v16qi_uuu(prev1_andF, lookup_b1l);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqtbl1q_v)          // Clang
                b1l = ::std::bit_cast<u8x16simd>(__builtin_neon_vqtbl1q_v(::std::bit_cast<i8x16simd>(prev1_andF), ::std::bit_cast<i8x16simd>(lookup_b1l)));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                b1l = ::std::bit_cast<u8x16simd>(__builtin_lsx_vshuf_b(lookup_b1l, lookup_b1l, prev1_andF));
# else
#  error "missing instructions"
# endif

                auto const inp_shr4{::std::bit_cast<u8x16simd>(::std::bit_cast<u16x8simd>(input) >> static_cast<::std::uint16_t>(4u)) &
                                    static_cast<::std::uint8_t>(0xFFu >> 4u)};

                u8x16simd b2h;
# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                b2h = ::std::bit_cast<u8x16simd>(__builtin_ia32_pshufb128(lookup_b2h, inp_shr4));
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_qtbl1v16qi_uuu)  // GCC
                b2h = __builtin_aarch64_qtbl1v16qi_uuu(inp_shr4, lookup_b2h);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqtbl1q_v)          // Clang
                b2h = ::std::bit_cast<u8x16simd>(__builtin_neon_vqtbl1q_v(::std::bit_cast<i8x16simd>(inp_shr4), ::std::bit_cast<i8x16simd>(lookup_b2h)));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                b2h = ::std::bit_cast<u8x16simd>(__builtin_lsx_vshuf_b(lookup_b2h, lookup_b2h, inp_shr4));
# else
#  error "missing instructions"
# endif

                return b1h & b1l & b2h;
            }};

        constexpr auto check_multibyte_lengths{
            [] UWVM_ALWAYS_INLINE(u8x16simd const input, u8x16simd const prev_input, u8x16simd const sc) constexpr noexcept -> u8x16simd
            {
                auto prev2{__builtin_shufflevector(prev_input,
                                                   input,
                                                   14,  // 0
                                                   15,  // 1
                                                   16,  // 2
                                                   17,  // 3
                                                   18,  // 4
                                                   19,  // 5
                                                   20,  // 6
                                                   21,  // 7
                                                   22,  // 8
                                                   23,  // 9
                                                   24,  // 10
                                                   25,  // 11
                                                   26,  // 12
                                                   27,  // 13
                                                   28,  // 14
                                                   29   // 15
                                                   )};

                auto prev3{__builtin_shufflevector(prev_input,
                                                   input,
                                                   13,  // 0
                                                   14,  // 1
                                                   15,  // 2
                                                   16,  // 3
                                                   17,  // 4
                                                   18,  // 5
                                                   19,  // 6
                                                   20,  // 7
                                                   21,  // 8
                                                   22,  // 9
                                                   23,  // 10
                                                   24,  // 11
                                                   25,  // 12
                                                   26,  // 13
                                                   27,  // 14
                                                   28   // 15
                                                   )};

                constexpr auto prev2_needsubs{static_cast<::std::uint8_t>(0xE0u - 0x80u)};
                u8x16simd const prev2_needsubs_simd{
                    prev2_needsubs,  // 0
                    prev2_needsubs,  // 1
                    prev2_needsubs,  // 2
                    prev2_needsubs,  // 3
                    prev2_needsubs,  // 4
                    prev2_needsubs,  // 5
                    prev2_needsubs,  // 6
                    prev2_needsubs,  // 7
                    prev2_needsubs,  // 8
                    prev2_needsubs,  // 9
                    prev2_needsubs,  // 10
                    prev2_needsubs,  // 11
                    prev2_needsubs,  // 12
                    prev2_needsubs,  // 13
                    prev2_needsubs,  // 14
                    prev2_needsubs   // 15
                };

# if UWVM_HAS_BUILTIN(__builtin_elementwise_sub_sat)  // Clang
                prev2 = __builtin_elementwise_sub_sat(prev2, prev2_needsubs_simd);
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_psubusb128)
                prev2 = __builtin_ia32_psubusb128(prev2, prev2_needsubs_simd);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_ussubv16qi_uuu)  // GCC
                prev2 = __builtin_aarch64_ussubv16qi_uuu(prev2, prev2_needsubs_simd);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqsubuv16qi)
                prev2 = ::std::bit_cast<u8x16simd>(::std::bit_cast<i8x16simd>(prev2), ::std::bit_cast<i8x16simd>(prev2_needsubs_simd));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vssub_bu)
                prev2 = __builtin_lsx_vssub_bu(prev2, prev2_needsubs_simd);
# else
#  error "missing instruction"
# endif

                constexpr auto prev3_needsubs{static_cast<::std::uint8_t>(0xF0u - 0x80u)};
                u8x16simd const prev3_needsubs_simd{
                    prev3_needsubs,  // 0
                    prev3_needsubs,  // 1
                    prev3_needsubs,  // 2
                    prev3_needsubs,  // 3
                    prev3_needsubs,  // 4
                    prev3_needsubs,  // 5
                    prev3_needsubs,  // 6
                    prev3_needsubs,  // 7
                    prev3_needsubs,  // 8
                    prev3_needsubs,  // 9
                    prev3_needsubs,  // 10
                    prev3_needsubs,  // 11
                    prev3_needsubs,  // 12
                    prev3_needsubs,  // 13
                    prev3_needsubs,  // 14
                    prev3_needsubs   // 15
                };

# if UWVM_HAS_BUILTIN(__builtin_elementwise_sub_sat)  // Clang
                prev3 = __builtin_elementwise_sub_sat(prev3, prev3_needsubs_simd);
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_psubusb128)
                prev3 = __builtin_ia32_psubusb128(prev3, prev3_needsubs_simd);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_ussubv16qi_uuu)  // GCC
                prev3 = __builtin_aarch64_ussubv16qi_uuu(prev3, prev3_needsubs_simd);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqsubuv16qi)
                prev3 = ::std::bit_cast<u8x16simd>(::std::bit_cast<i8x16simd>(prev3), ::std::bit_cast<i8x16simd>(prev3_needsubs_simd));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vssub_bu)
                prev3 = __builtin_lsx_vssub_bu(prev3, prev3_needsubs_simd);
# else
#  error "missing instruction"
# endif

                return ((prev2 | prev3) & static_cast<::std::uint8_t>(0x80u)) ^ sc;
            }};

        constexpr auto check_utf8_bytes{
            [check_special_cases, check_multibyte_lengths] UWVM_ALWAYS_INLINE(u8x16simd const input, u8x16simd const prev_input) constexpr noexcept -> u8x16simd
            {
                auto const prev1{__builtin_shufflevector(prev_input,
                                                         input,
                                                         15,  // 0
                                                         16,  // 1
                                                         17,  // 2
                                                         18,  // 3
                                                         19,  // 4
                                                         20,  // 5
                                                         21,  // 6
                                                         22,  // 7
                                                         23,  // 8
                                                         24,  // 9
                                                         25,  // 10
                                                         26,  // 11
                                                         27,  // 12
                                                         28,  // 13
                                                         29,  // 14
                                                         30   // 15
                                                         )};

                auto const sc{check_special_cases(input, prev1)};
                return check_multibyte_lengths(input, prev_input, sc);
            }};

        constexpr auto is_incomplete{[] UWVM_ALWAYS_INLINE(u8x16simd input) constexpr noexcept -> u8x16simd
                                     {
                                         constexpr u8x16simd max_value{
                                             255u,              // 0
                                             255u,              // 1
                                             255u,              // 2
                                             255u,              // 3
                                             255u,              // 4
                                             255u,              // 5
                                             255u,              // 6
                                             255u,              // 7
                                             255u,              // 8
                                             255u,              // 9
                                             255u,              // 10
                                             255u,              // 11
                                             255u,              // 12
                                             0b11110000u - 1u,  // 13
                                             0b11100000u - 1u,  // 14
                                             0b11000000u - 1u   // 15
                                         };

# if UWVM_HAS_BUILTIN(__builtin_elementwise_sub_sat)  // Clang
                                         input = __builtin_elementwise_sub_sat(input, max_value);
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_psubusb128)
                                         input = __builtin_ia32_psubusb128(input, max_value);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_ussubv16qi_uuu)  // GCC
                                         input = __builtin_aarch64_ussubv16qi_uuu(input, max_value);
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vqsubuv16qi)
                                         input = ::std::bit_cast<u8x16simd>(::std::bit_cast<i8x16simd>(input), ::std::bit_cast<i8x16simd>(max_value));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vssub_bu)
                                         input = __builtin_lsx_vssub_bu(input, max_value);
# else
#  error "missing instruction"
# endif

                                         return input;
                                     }};

        u8x16simd error{};
        u8x16simd prev_input_block{};
        u8x16simd prev_incomplete{};

        while(static_cast<::std::size_t>(str_end - str_curr) >= sizeof(u8x16simd) * 4uz)
        {
            u8x16simd curr0;
            u8x16simd curr1;
            u8x16simd curr2;
            u8x16simd curr3;

            ::std::memcpy(::std::addressof(curr0), str_curr + sizeof(u8x16simd) * 0uz, sizeof(u8x16simd));
            ::std::memcpy(::std::addressof(curr1), str_curr + sizeof(u8x16simd) * 1uz, sizeof(u8x16simd));
            ::std::memcpy(::std::addressof(curr2), str_curr + sizeof(u8x16simd) * 2uz, sizeof(u8x16simd));
            ::std::memcpy(::std::addressof(curr3), str_curr + sizeof(u8x16simd) * 3uz, sizeof(u8x16simd));

            // check null
            if constexpr(zero_illegal)
            {
                auto const curr0_test{curr0 == static_cast<::std::uint8_t>(0u)};

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    __builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(curr0_test), ::std::bit_cast<i64x2simd>(curr0_test))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    !__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(curr0_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    __builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~curr0_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    !__builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(curr0_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                    !__builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(curr0_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                    !__builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(curr0_test))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    !__builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(curr0_test))  /// @todo need check
# else
#  error "missing instructions"
# endif
                        ) [[unlikely]]
                {
                    // Jump out to scalar processing
                    break;
                }

                auto const curr1_test{curr1 == static_cast<::std::uint8_t>(0u)};

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    __builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(curr1_test), ::std::bit_cast<i64x2simd>(curr1_test))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    !__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(curr1_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    __builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~curr1_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    !__builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(curr1_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                    !__builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(curr1_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                    !__builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(curr1_test))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    !__builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(curr1_test))  /// @todo need check
# else
#  error "missing instructions"
# endif
                        ) [[unlikely]]
                {
                    // Jump out to scalar processing
                    break;
                }

                auto const curr2_test{curr2 == static_cast<::std::uint8_t>(0u)};

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    __builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(curr2_test), ::std::bit_cast<i64x2simd>(curr2_test))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    !__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(curr2_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    __builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~curr2_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    !__builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(curr2_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                    !__builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(curr2_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                    !__builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(curr2_test))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    !__builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(curr2_test))  /// @todo need check
# else
#  error "missing instructions"
# endif
                        ) [[unlikely]]
                {
                    // Jump out to scalar processing
                    break;
                }

                auto const curr3_test{curr3 == static_cast<::std::uint8_t>(0u)};

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    __builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(curr3_test), ::std::bit_cast<i64x2simd>(curr3_test))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    !__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(curr3_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    __builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~curr3_test))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    !__builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(curr3_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                    !__builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(curr3_test))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                    !__builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(curr3_test))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    !__builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(curr3_test))  /// @todo need check
# else
#  error "missing instructions"
# endif
                        ) [[unlikely]]
                {
                    // Jump out to scalar processing
                    break;
                }
            }

            // check ascii

            auto const check_upper{(curr0 | curr1 | curr2 | curr3) > static_cast<::std::uint8_t>(0x7Fu)};

            if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
            )
            {
                // utf-8

                error |= check_utf8_bytes(curr0, prev_input_block);
                error |= check_utf8_bytes(curr1, curr0);
                error |= check_utf8_bytes(curr2, curr1);
                error |= check_utf8_bytes(curr3, curr2);

                prev_incomplete = is_incomplete(curr3);
                prev_input_block = curr3;
            }
            else
            {
                // ascii

                error |= prev_incomplete;
            }

            if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(error), ::std::bit_cast<i64x2simd>(error))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(error))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~error))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(error))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(error))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(error))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(error))  /// @todo need check
# else
#  error "missing instructions"
# endif
                    ) [[unlikely]]
            {
                // Jump out to scalar processing
                break;
            }

            str_curr += sizeof(u8x16simd) * 4uz;

            // prefetch
            ::uwvm2::utils::intrinsics::universal::prefetch<::uwvm2::utils::intrinsics::universal::pfc_mode::read,
                                                            ::uwvm2::utils::intrinsics::universal::pfc_level::L2,
                                                            ::uwvm2::utils::intrinsics::universal::ret_policy::keep>(
                reinterpret_cast<::std::byte const*>(str_curr) + 64u);
        }

        // tail handling

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
                if(static_cast<::std::size_t>(str_end - str_curr) < 2u) [[unlikely]]
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
                utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
                utf8_b0 <<= (utf8_length - 1u) * 6u;

                ::std::uint_least32_t utf8_b1{static_cast<::std::uint_least32_t>(str_curr_p1_val)};
                utf8_b1 &= 0x3Fu;
                utf8_b1 <<= (utf8_length - 2u) * 6u;

                char32_t const utf8_c{static_cast<char32_t>(utf8_b0 | utf8_b1)};

                constexpr auto test_overlong{utf8_length - 2u};
                constexpr auto test_overlong_pc{(test_overlong + 1u) >> 1u};

                constexpr auto test_overlong_low_bound{1u << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                {
                    return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                }

                str_curr += utf8_length;
            }
            else if((str_curr_val & static_cast<char8_t>(0b1111'0000u)) == static_cast<char8_t>(0b1110'0000u))
            {
                if(static_cast<::std::size_t>(str_end - str_curr) < 3u) [[unlikely]]
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
                utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
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

                constexpr auto test_overlong_low_bound{1u << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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
                if(static_cast<::std::size_t>(str_end - str_curr) < 4u) [[unlikely]]
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
                utf8_b0 &= ((1u << utf8_b0_shu1) - 1u);
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

                constexpr auto test_overlong_low_bound{1u << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                {
                    return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                }

                str_curr += utf8_length;
            }
            else
            {
                return {str_curr, ::uwvm2::utils::utf::utf_error_code::long_header_bits};
            }
        }

        return {str_curr, ::uwvm2::utils::utf::utf_error_code::success};

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__SSE2__) || defined(__wasm_simd128__))

        // Algorithm author MacroModel

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

                ::std::memcpy(::std::addressof(need_check), str_curr, sizeof(u8x16simd));

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

                ::std::memcpy(::std::addressof(utf_8), str_curr, sizeof(::std::uint_least32_t));

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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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
        // Algorithm author MacroModel

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

                    ::std::memcpy(::std::addressof(need_check1), str_curr, sizeof(::std::uint_least64_t));

                    ::std::uint_least64_t need_check2;

                    ::std::memcpy(::std::addressof(need_check2), str_curr + sizeof(::std::uint_least64_t), sizeof(::std::uint_least64_t));

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

                    ::std::memcpy(::std::addressof(utf_8), str_curr, sizeof(::std::uint_least32_t));

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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                        constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                               << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                                   << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                                   << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

                            constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                                   << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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
            // Compatible with platforms where CHAR_BIT is greater than 8, because it is guaranteed that only a meaningful portion of each byte operation is
            // fetched

            while(str_curr != str_end)
            {
                // [xx] xx xx ... (end)
                // [  ] unsafe (could be the section_end)
                //  ^^ section_curr

                auto str_curr_val{*str_curr};

                // A standards-compliant UTF-8 decoder must only care about the low 8 bits of each byte when verifying or decoding a byte sequence.

                // Subsequent judgment is required for < static_cast<char8_t>(0b1000'0000u), so for platforms where CHAR_BIT is not equal to 8 the high bit
                // needs to be cleared.

                if constexpr(!char_bit_is_8) { str_curr_val &= 0xFFu; }

                if(str_curr_val < static_cast<char8_t>(0b1000'0000u))
                {
                    // ??0xxxxxxx

                    if constexpr(zero_illegal)
                    {
                        if(str_curr_val == u8'\0') [[unlikely]] { return {str_curr, ::uwvm2::utils::utf::utf_error_code::contains_empty_characters}; }
                    }

                    ++str_curr;
                }
                else if((str_curr_val & static_cast<char8_t>(0b1110'0000u)) == static_cast<char8_t>(0b1100'0000u))
                {
                    // ??110xxxxx ??10xxxxxx

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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
                    if(utf8_c < static_cast<char32_t>(test_overlong_low_bound)) [[unlikely]]
                    {
                        return {str_curr, ::uwvm2::utils::utf::utf_error_code::overlong_encoding};
                    }

                    str_curr += utf8_length;
                }
                else if((str_curr_val & static_cast<char8_t>(0b1111'0000u)) == static_cast<char8_t>(0b1110'0000u))
                {
                    // ??1110xxxx ??10xxxxxx ??10xxxxxx

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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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
                    // ??11110xxx ??10xxxxxx ??10xxxxxx ??10xxxxxx

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

                    constexpr auto test_overlong_low_bound{static_cast<::std::uint_least32_t>(1u)
                                                           << (6u * test_overlong + (8u - utf8_length + (1u - test_overlong_pc)))};
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

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
