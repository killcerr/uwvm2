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
# include <concepts>
# include <bit>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

#if CHAR_BIT == 8
/// @brief "XXH3 only support CHAR_BIT == 8"

UWVM_MODULE_EXPORT namespace uwvm2::utils::hash
{
    namespace details
    {
        enum class xxh3_width_t : unsigned
        {
            XX3H_64,
            XX3H_128
        }

# if !defined(__SIZEOF_INT128__)
        struct xxh3_128_storage_t
        {
            // For msvc, which targets little-endian, msvc does not have the uint128 extension.
            // Only the uint64_t is guaranteed to be 64 bits.
            static_assert(::std::endian::native == ::std::endian::little);
            ::std::uint64_t low64;
            ::std::uint64_t high64;
        };
# else
        using xxh3_128_storage_t = __uint128_t;
# endif

        template <xxh3_width_t Wid>
            requires (static_cast<unsigned>(Wid) <= static_cast<unsigned>(XX3H_128))
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

        template <xxh3_width_t Wid>
            requires (static_cast<unsigned>(Wid) <= static_cast<unsigned>(XX3H_128))
        struct basic_xxh3_context
        {
            using digest_t = xxh3_output_struct<Wid>;

            inline static constexpr ::std::size_t digest_size{sizeof(digest_t)};

            inline constexpr void update(::std::byte const* first, ::std::byte const* last) noexcept {}

            inline constexpr void reset() noexcept {}

            inline constexpr digest_t digest_value() const noexcept {}

            inline constexpr void do_final() const noexcept {}

            inline constexpr void digest_to_byte_ptr(::std::byte* ptr) const noexcept {}
        };
    }  // namespace details

    using xxh3_64_context = details::basic_xxh3_context<details::xxh3_width_t::XX3H_64>;
    using xxh3_128_context = details::basic_xxh3_context<details::xxh3_width_t::XX3H_128>;
}
#endif

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
