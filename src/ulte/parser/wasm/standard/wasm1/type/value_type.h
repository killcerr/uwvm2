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
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::parser::wasm::standard::wasm1::type
{
    /// @brief      Bytes
    /// @details    The simplest form of value are raw uninterpreted bytes. In the abstract syntax they are represented as hexadecimal
    ///             literals.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.2.1
    using wasm_byte = ::std::uint_least8_t;

    /// @brief      Integers
    /// @details    Different classes of integers with different value ranges are distinguished by their bit width N and by whether they
    ///             are unsigned or signed.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.2.2
    using wasm_i8 = ::std::int_least8_t;   // nonstandard
    using wasm_u8 = ::std::uint_least8_t;  // nonstandard

    using wasm_i16 = ::std::int_least16_t;   // nonstandard
    using wasm_u16 = ::std::uint_least16_t;  // nonstandard

    using wasm_i32 = ::std::int_least32_t;
    using wasm_u32 = ::std::uint_least32_t;

    using wasm_i64 = ::std::int_least64_t;
    using wasm_u64 = ::std::uint_least64_t;

    /// @brief      Floating-Point
    /// @details    Floating-point data represents 32 or 64 bit values that correspond to the respective binary formats of the IEEE
    ///             754-20089 standard (Section 3.3).
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.2.3
#ifdef __STDCPP_FLOAT32_T__
    using wasm_f32 = _Float32;  // IEEE 754-2008
#else
    using wasm_f32 = float;
#endif

#ifdef __STDCPP_FLOAT64_T__
    using wasm_f64 = _Float64;  // IEEE 754-2008
#else
    using wasm_f64 = double;
#endif

    /// @brief      wasm base instruction, using a byte composition, instruction or extension instruction.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.4.5
    using op_basic_type = wasm_byte;

    /// @brief      wasm extended instructions, using u32, indicate the extended instruction section.
    /// @details    New feature
    /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.4.5
    using op_exten_type = wasm_u32;

    // func
    namespace details
    {
        /// @brief      Type of varint converted to fixed-length int
        /// @see        WebAssembly Release 1.0 (2019-07-20) § 5.2.2
        template <::std::size_t Varint, bool Unsigned>
            requires (0 < Varint && Varint <= 64)
        inline consteval auto get_varint_type_from_type_bit_size_impl() noexcept
        {
            if constexpr(Varint <= 8)
            {
                if constexpr(Unsigned) { return wasm_u8{}; }
                else { return wasm_i8{}; }
            }
            else if constexpr(Varint <= 16)
            {
                if constexpr(Unsigned) { return wasm_u16{}; }
                else { return wasm_i16{}; }
            }
            else if constexpr(Varint <= 32)
            {
                if constexpr(Unsigned) { return wasm_u32{}; }
                else { return wasm_i32{}; }
            }
            else if constexpr(Varint <= 64)
            {
                if constexpr(Unsigned) { return wasm_u64{}; }
                else { return wasm_i64{}; }
            }
        }

        /// @brief      Getting Variable Types from Maximum Storage Capacity
        /// @details    All integers are encoded using the LEB12829 variable-length integer encoding, in either unsigned or signed variant.
        ///             Unsigned integers are encoded in unsigned LEB12830 format. As an additional constraint, the total number of
        ///             bytes encoding a value of type uN must not exceed ceil(N / 7) bytes
        ///             Signed integers are encoded in signed LEB12831 format, which uses a two’s complement representation. As an
        ///             additional constraint, the total number of bytes encoding a value of type sN must not exceed ceil(N / 7) bytes.
        /// @see        WebAssembly Release 1.0 (2019-07-20) § 5.2.2
        template <::std::size_t StorageSize, bool Unsigned>
            requires (0 < StorageSize && StorageSize <= 10)
        inline consteval auto get_varint_type_from_max_storage_size_impl() noexcept
        {
            if constexpr(StorageSize <= 1)
            {
                if constexpr(Unsigned) { return wasm_u8{}; }
                else { return wasm_i8{}; }
            }
            else if constexpr(StorageSize <= 3)
            {
                if constexpr(Unsigned) { return wasm_u16{}; }
                else { return wasm_i16{}; }
            }
            else if constexpr(StorageSize <= 5)
            {
                if constexpr(Unsigned) { return wasm_u32{}; }
                else { return wasm_i32{}; }
            }
            else if constexpr(StorageSize <= 10)
            {
                if constexpr(Unsigned) { return wasm_u64{}; }
                else { return wasm_i64{}; }
            }
        }
    }  // namespace details

    template <::std::size_t Varint, bool Unsigned>
    using varint_type_from_type_bsz = decltype(details::get_varint_type_from_type_bit_size_impl<Varint, Unsigned>());

    template <::std::size_t StorageSize, bool Unsigned>
    using varint_type_from_max_ssz = decltype(details::get_varint_type_from_max_storage_size_impl<StorageSize, Unsigned>());

}  // namespace ulte::parser::wasm::standard::wasm1::type
