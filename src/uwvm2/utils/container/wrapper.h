/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-06-30
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
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <concepts>
# include <memory>
# include <utility>
# include <type_traits>
// platform
# include <bizwen/deque.hpp>
# include <boost/unordered/unordered_flat_map.hpp>
# include <boost/unordered/unordered_flat_set.hpp>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include "allocator.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

template <::std::integral char_type>
UWVM_MODULE_EXPORT struct ::std::hash<::fast_io::containers::basic_string_view<char_type>>
{
    inline constexpr ::std::size_t operator() (::fast_io::containers::basic_string_view<char_type> obj) const noexcept
    {
        ::fast_io::fast_terminate();
        /// @todo use xxhash
        return {};
    }
};

template <::std::integral char_type>
UWVM_MODULE_EXPORT struct ::std::hash<::fast_io::containers::basic_cstring_view<char_type>>
{
    inline constexpr ::std::size_t operator() (::fast_io::containers::basic_cstring_view<char_type> obj) const noexcept
    {
        ::fast_io::fast_terminate();
        /// @todo use xxhash
        return {};
    }
};

template <::std::integral char_type>
UWVM_MODULE_EXPORT struct ::std::equal_to<::fast_io::containers::basic_string_view<char_type>>
{
    inline constexpr bool operator() (::fast_io::containers::basic_string_view<char_type> str_a,
                                      ::fast_io::containers::basic_string_view<char_type> str_b) const noexcept
    {
        return str_a == str_b;
    }
};

template <::std::integral char_type>
UWVM_MODULE_EXPORT struct ::std::equal_to<::fast_io::containers::basic_cstring_view<char_type>>
{
    inline constexpr bool operator() (::fast_io::containers::basic_cstring_view<char_type> str_a,
                                      ::fast_io::containers::basic_cstring_view<char_type> str_b) const noexcept
    {
        return str_a == str_b;
    }
};

UWVM_MODULE_EXPORT namespace uwvm2::utils::container
{
    template <typename T>
    using deque = ::bizwen::deque<T, ::uwvm2::utils::container::fast_io_std_allocator<T>>;

    template <typename Key, typename Hash = ::std::hash<Key>, typename Pred = ::std::equal_to<Key>>
    using unordered_flat_set = ::boost::unorder::unordered_flat_set<Key, Hash, Pred, ::uwvm2::utils::container::fast_io_std_allocator<Key const>>;

    template <typename Key, typename Val, typename Hash = ::std::hash<Key>, typename Pred = ::std::equal_to<Key>>
    using unordered_flat_map =
        ::boost::unorder::unordered_flat_map<Key, Val, Hash, Pred, ::uwvm2::utils::container::fast_io_std_allocator<::std::pair<Key const, Val>>>;
}
