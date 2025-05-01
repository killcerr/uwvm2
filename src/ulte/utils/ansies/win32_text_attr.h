/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-24
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

#if defined(_WIN32) && (_WIN32_WINNT < 0x0A00 || defined(_WIN32_WINDOWS))

# ifdef UWVM_MODULE
import fast_io;
# else
// std
#  include <cstdint>
#  include <cstddef>
#  include <concepts>
// import
#  include <fast_io.h>
# endif

# ifndef UWVM_MODULE_EXPORT
#  define UWVM_MODULE_EXPORT
# endif

UWVM_MODULE_EXPORT namespace ulte::utils::ansies
{
    enum class text_attr : ::std::int_least32_t
    {
        foreground_blue = 0x0001,   // text color contains blue.
        foreground_green = 0x0002,  // text color contains green.
        foreground_red = 0x0004,    // text color contains red.

        foreground_intensity = 0x0008,  // text color is intensified.

        background_blue = 0x0010,   // background color contains blue.
        background_green = 0x0020,  // background color contains green.
        background_red = 0x0040,    // background color contains red.

        background_intensity = 0x0080,  // background color is intensified.

        common_lvb_leading_byte = 0x0100,     // leading byte of dbcs
        common_lvb_trailing_byte = 0x0200,    // trailing byte of dbcs
        common_lvb_grid_horizontal = 0x0400,  // dbcs: grid attribute: top horizontal.
        common_lvb_grid_lvertical = 0x0800,   // dbcs: grid attribute: left vertical.
        common_lvb_grid_rvertical = 0x1000,   // dbcs: grid attribute: right vertical.
        common_lvb_reverse_video = 0x4000,    // dbcs: reverse fore/back ground attribute.
        common_lvb_underscore = 0x8000,       // dbcs: underscore.

        common_lvb_sbcsdbcs = 0x0300,  // sbcs or dbcs flag.

        // custom foreground
        foreground_black = 0x0000,
        foreground_orange = 0x0002 | 0x0004,
        foreground_purple = 0x0001 | 0x0004,
        foreground_cyan = 0x0001 | 0x0002,
        foreground_gray = 0x0001 | 0x0002 | 0x0004,

        foreground_dk_gray = 0x0008 | 0x0000,
        foreground_lt_red = 0x0008 | 0x0004,
        foreground_lt_green = 0x0008 | 0x0002,
        foreground_yellow = 0x0008 | 0x0002 | 0x0004,
        foreground_lt_blue = 0x0008 | 0x0001,
        foreground_lt_purple = 0x0008 | 0x0001 | 0x0004,
        foreground_lt_cyan = 0x0008 | 0x0001 | 0x0002,
        foreground_white = 0x0008 | 0x0001 | 0x0002 | 0x0004,

        foreground_rst_all = foreground_gray,
    };

    inline constexpr text_attr operator& (text_attr x, text_attr y) noexcept
    {
        using utype = typename ::std::underlying_type<text_attr>::type;
        return static_cast<text_attr>(static_cast<utype>(x) & static_cast<utype>(y));
    }

    inline constexpr text_attr operator| (text_attr x, text_attr y) noexcept
    {
        using utype = typename ::std::underlying_type<text_attr>::type;
        return static_cast<text_attr>(static_cast<utype>(x) | static_cast<utype>(y));
    }

    inline constexpr text_attr operator^ (text_attr x, text_attr y) noexcept
    {
        using utype = typename ::std::underlying_type<text_attr>::type;
        return static_cast<text_attr>(static_cast<utype>(x) ^ static_cast<utype>(y));
    }

    inline constexpr text_attr operator~(text_attr x) noexcept
    {
        using utype = typename ::std::underlying_type<text_attr>::type;
        return static_cast<text_attr>(~static_cast<utype>(x));
    }

    inline constexpr text_attr& operator&= (text_attr& x, text_attr y) noexcept { return x = x & y; }

    inline constexpr text_attr& operator|= (text_attr& x, text_attr y) noexcept { return x = x | y; }

    inline constexpr text_attr& operator^= (text_attr& x, text_attr y) noexcept { return x = x ^ y; }

    struct win32_text_attr
    {
        void* handle{};
        text_attr attr{};
    };

    template <::std::integral char_type, typename s>
    inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, win32_text_attr>, s&&, win32_text_attr attr) noexcept
    {
        ::fast_io::win32::SetConsoleTextAttribute(attr.handle, static_cast<::std::int_least32_t>(attr.attr));
    }
}  // namespace ulte::utils::ansies

#endif
