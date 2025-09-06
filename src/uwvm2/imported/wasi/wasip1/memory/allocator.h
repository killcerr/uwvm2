
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
# include <climits>
# include <limits>
# include <concepts>
# include <bit>
// import
# include <fast_io.h>
# include <uwvm2/object/memory/impl.h>
# include <uwvm2/imported/wasi/wasip1/abi/impl.h>
# include <uwvm2/imported/wasi/wasip1/fd_manager/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::memory
{

#if CHAR_BIT != 8
# error "CHAR_BIT must be 8"
#endif

    template <typename WasmType>
    struct get_basic_wasm_type_from_memory_ret_t
    {
        ::uwvm2::imported::wasi::wasip1::abi::errno_t error_code;
        WasmType value;
    };

    /// @brief      Read a Wasm value from linear memory (allocator backend) with concurrency safety and bounds double-checking.
    /// @details    - Concurrency safety: use the double-atom guard (growing_flag_p + active_ops_p) to synchronize with grow operations.
    ///             - Bounds double-checks:
    ///               (1) Using the size_t parameter supports both u32 and u64. Comparison checks between u32 or u64 and size_t will be performed by the calling
    ///                   function.
    ///               (2) Compare with the actual memory length to prevent out-of-bounds access.
    ///             - Endianness: decode as little-endian; byteswap on big-endian hosts, PDP-11 Segment Sequence and Non-8-Bit Charbit Platform-Specific
    ///                   Handling.
    template <typename WasmType, typename Alloc>
    inline constexpr get_basic_wasm_type_from_memory_ret_t<WasmType> get_basic_wasm_type_from_memory(
        ::uwvm2::object::memory::linear::basic_allocator_memory_t<Alloc> const& memory,
        ::std::size_t offset) noexcept
    {
        // Number of 8-bit WASM bytes needed to represent WasmType
        constexpr ::std::size_t wasm_bytes{sizeof(WasmType)};

        // Mutual exclusion between concurrent read/write operations and memory growth: Entering the memory operation region
        ::uwvm2::object::memory::linear::memory_operation_guard_t memory_op_guard{memory.growing_flag_p, memory.active_ops_p};

        // Conduct a full inspection of the memory.
        auto const memory_begin{memory.memory_begin};

        if(memory_begin == nullptr) [[unlikely]] { return {::uwvm2::imported::wasi::wasip1::abi::errno_t::efault, {}}; }

        auto const memory_length{memory.memory_length};

        // The remaining size does not support reading a single type.
        if(wasm_bytes > memory_length) [[unlikely]] { return {::uwvm2::imported::wasi::wasip1::abi::errno_t::efault, {}}; }

        // Can read N bytes at the last legal position
        if(offset > (memory_length - wasm_bytes)) [[unlikely]] { return {::uwvm2::imported::wasi::wasip1::abi::errno_t::efault, {}}; }

        if constexpr(::std::integral<WasmType>)
        {
            using unsigned_wasm_type = ::std::make_unsigned_t<WasmType>;
            unsigned_wasm_type u;  // no init required

            // never overflow
            ::std::memcpy(::std::addressof(u), memory_begin + offset, sizeof(u));

            u = ::fast_io::little_endian(u);

            return {::uwvm2::imported::wasi::wasip1::abi::errno_t::esuccess, static_cast<WasmType>(u)};
        }
        else
        {
            static_assert(::std::integral<WasmType>, "wasi only supports the use of integer types.");
        }

        return {::uwvm2::imported::wasi::wasip1::abi::errno_t::efault, {}};
    }

}  // namespace uwvm2::imported::wasi::wasip1::memory

