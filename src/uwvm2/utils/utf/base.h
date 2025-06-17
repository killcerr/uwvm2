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
#else
// std
# include <cstdint>
# include <cstddef>
# include <concepts>
# include <memory>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::utf
{
    enum class utf_error_code : unsigned
    {
        success = 0u,
        long_header_bits,
        too_short_sequence,
        too_long_sequence,
        overlong_encoding,
        excessive_codepoint,
        illegal_surrogate
    };

    template <::std::integral char_type>
    struct result_base
    {
        char_type const* pos {}
        utf_error_code err{};
    };

    using result = result_base<char>;
    using wresult = result_base<wchar_t>;
    using u8result = result_base<char8_t>;
    using u16result = result_base<char16_t>;
    using u32result = result_base<char32_t>;

}  // namespace uwvm2::utils::utf
