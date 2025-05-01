﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-31
 * @copyright   APL-2 License
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
import ulte.parser.wasm.standard.wasm1;
#else
// std
# include <cstdint>
# include <cstddef>
# include <concepts>
# include <bit>
// macro
# include <ulte/parser/wasm/feature/feature_push_macro.h>
// import
# include <fast_io.h>
# include <ulte/parser/wasm/standard/wasm1/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1p1::type
{
    /// @brief      Vector Types
    /// @details    Vector types classify vectors of numeric values processed by vector instructions (also known as SIMD instructions,
    ///             single instruction multiple data).
    ///             This is only used for storage and will be converted to the type used for computation during computation depending on platform support
    /// @see        WebAssembly Release 1.1 (Draft 2021-11-16) § 2.3.2
#if __has_cpp_attribute(__gnu__::__vector_size__)
    using wasm_v128 [[__gnu__::__vector_size__(16)]] = char;
#else
# if defined(_MSC_VER) && !defined(__clang__)
    __declspec(align(16))
# endif
    union
# if __has_cpp_attribute(__gnu__::__aligned__)
        [[__gnu__::__aligned__(16)]]
# endif
        wasm_v128
    {
        wasm_u8 u8x16[16];
        wasm_i8 i8x16[16];

        wasm_u16 u16x8[8];
        wasm_i16 i16x8[8];

        wasm_u32 u32x4[4];
        wasm_i32 i32x4[4];
        wasm_f32 f32x4[4];

        wasm_u64 u64x2[2];
        wasm_i64 i64x2[2];
        wasm_f64 f64x2[2];
    };
#endif

    // func

    /// @brief      v128 print_reserve
    /// @details    output: "[00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0a, 0b, 0c, 0d, 0e, 0f]"
    template <::std::integral char_type>
    inline constexpr ::std::size_t print_reserve_size(::fast_io::io_reserve_type_t<char_type, wasm_v128>) noexcept
    {
        constexpr auto hex_real_size{::fast_io::pr_rsv_size<char_type, decltype(::fast_io::mnp::hexupper<false, true>(::std::uint_least8_t{}))>};
        return hex_real_size * 16 + 15 * 2 + 2;  // hex * 16 + 15 * ',' + 15 * ' ' + '[' + ']'
    }

    namespace details
    {
        template <::std::integral char_type>
        inline constexpr char_type* print_reserve_v128_i8x16_impl(char_type* iter, wasm_v128 v128_val) noexcept
        {
            constexpr auto space{::fast_io::char_literal_v<u8' ', char_type>};
            constexpr auto comma{::fast_io::char_literal_v<u8',', char_type>};

            auto const v128_i8x16_arr = ::std::bit_cast<::fast_io::array<::std::uint_least8_t, 16u>>(v128_val);
            auto const v128_i8x16{v128_i8x16_arr.cbegin()};
            char_type* curr_pos{iter};

            *curr_pos = ::fast_io::char_literal_v<u8'[', char_type>;
            ++curr_pos;

            if constexpr(::std::endian::native == ::std::endian::little)  // little endian
            {
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[0]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[1]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[2]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[3]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[4]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[5]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[6]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[7]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[8]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[9]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[10]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[11]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[12]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[13]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[14]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[15]));
            }
            else if constexpr(::std::endian::native == ::std::endian::big)  // big endian
            {
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[15]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[14]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[13]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[12]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[11]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[10]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[9]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[8]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[7]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[6]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[5]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[4]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[3]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[2]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[1]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[0]));
            }
            else  // PDP-endian
            {
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[14]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[15]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[12]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[13]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[10]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[11]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[8]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[9]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[6]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[7]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[4]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[5]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[2]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[3]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[0]));
                *curr_pos = comma;
                ++curr_pos;
                *curr_pos = space;
                ++curr_pos;
                curr_pos = ::fast_io::pr_rsv_to_iterator_unchecked(curr_pos, ::fast_io::mnp::hexupper<false, true>(v128_i8x16[1]));
            }
            *curr_pos = ::fast_io::char_literal_v<u8']', char_type>;
            return ++curr_pos;
        }
    }  // namespace details

    template <::std::integral char_type>
    inline constexpr char_type* print_reserve_define(::fast_io::io_reserve_type_t<char_type, wasm_v128>, char_type * iter, wasm_v128 v128_val) noexcept
    {
        return details::print_reserve_v128_i8x16_impl(iter, v128_val);
    }
}  // namespace ulte::parser::wasm::standard::wasm1p1::type
