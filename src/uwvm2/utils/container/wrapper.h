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
# include <climits>
# include <concepts>
# include <memory>
# include <utility>
# include <type_traits>
# include <string_view>
# include <set>  /// @todo replace with btreemap
# include <map>  /// @todo replace with btreemap
// platform
# include <bizwen/deque.hpp>
# include <boost/unordered/unordered_flat_map.hpp>
# include <boost/unordered/unordered_flat_set.hpp>
# include <boost/unordered/unordered_node_map.hpp>
# include <boost/unordered/unordered_node_set.hpp>
// import
# include <fast_io.h>
# include <fast_io_dsal/tuple.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/vector.h>
# include <uwvm2/utils/hash/impl.h>
# include "allocator.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::container
{
    /// @brief tuple
    using ::fast_io::containers::forward_as_tuple;
    using ::fast_io::containers::get;
    using ::fast_io::containers::is_tuple;
    using ::fast_io::containers::tuple;

    /// @brief array
    template <typename T, ::std::size_t N>
    using array = ::fast_io::containers::array<T, N>;

    /// @brief string_view
    template <::std::integral chartype>
    using basic_string_view = ::fast_io::containers::basic_string_view<chartype>;

    using string_view = ::uwvm2::utils::container::basic_string_view<char>;
    using wstring_view = ::uwvm2::utils::container::basic_string_view<wchar_t>;
    using u8string_view = ::uwvm2::utils::container::basic_string_view<char8_t>;
    using u16string_view = ::uwvm2::utils::container::basic_string_view<char16_t>;
    using u32string_view = ::uwvm2::utils::container::basic_string_view<char32_t>;

    /// @brief cstring_view
    template <::std::integral chartype>
    using basic_cstring_view = ::fast_io::containers::basic_cstring_view<chartype>;

    using cstring_view = ::uwvm2::utils::container::basic_cstring_view<char>;
    using wcstring_view = ::uwvm2::utils::container::basic_cstring_view<wchar_t>;
    using u8cstring_view = ::uwvm2::utils::container::basic_cstring_view<char8_t>;
    using u16cstring_view = ::uwvm2::utils::container::basic_cstring_view<char16_t>;
    using u32cstring_view = ::uwvm2::utils::container::basic_cstring_view<char32_t>;

    /// @brief string
    template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
    using basic_string = ::fast_io::containers::basic_string<chartype, allocator>;

    using string = ::uwvm2::utils::container::basic_string<char>;
    using wstring = ::uwvm2::utils::container::basic_string<wchar_t>;
    using u8string = ::uwvm2::utils::container::basic_string<char8_t>;
    using u16string = ::uwvm2::utils::container::basic_string<char16_t>;
    using u32string = ::uwvm2::utils::container::basic_string<char32_t>;

    template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
    using basic_ostring_ref_uwvm = ::fast_io::io_strlike_reference_wrapper<chartype, ::uwvm2::utils::container::basic_string<chartype, allocator>>;

    using ostring_ref_uwvm = ::uwvm2::utils::container::basic_ostring_ref_uwvm<char>;
    using wostring_ref_uwvm = ::uwvm2::utils::container::basic_ostring_ref_uwvm<wchar_t>;
    using u8ostring_ref_uwvm = ::uwvm2::utils::container::basic_ostring_ref_uwvm<char8_t>;
    using u16ostring_ref_uwvm = ::uwvm2::utils::container::basic_ostring_ref_uwvm<char16_t>;
    using u32ostring_ref_uwvm = ::uwvm2::utils::container::basic_ostring_ref_uwvm<char32_t>;

    namespace tlc
    {
        template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
        using basic_string = ::uwvm2::utils::container::basic_string<chartype, allocator>;

        using string = ::uwvm2::utils::container::tlc::basic_string<char>;
        using wstring = ::uwvm2::utils::container::tlc::basic_string<wchar_t>;
        using u8string = ::uwvm2::utils::container::tlc::basic_string<char8_t>;
        using u16string = ::uwvm2::utils::container::tlc::basic_string<char16_t>;
        using u32string = ::uwvm2::utils::container::tlc::basic_string<char32_t>;

        template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
        using basic_ostring_ref_uwvm_tlc = ::fast_io::io_strlike_reference_wrapper<chartype, ::uwvm2::utils::container::tlc::basic_string<chartype, allocator>>;

        using ostring_ref_uwvm_tlc = basic_ostring_ref_uwvm_tlc<char>;
        using wostring_ref_uwvm_tlc = basic_ostring_ref_uwvm_tlc<wchar_t>;
        using u8ostring_ref_uwvm_tlc = basic_ostring_ref_uwvm_tlc<char8_t>;
        using u16ostring_ref_uwvm_tlc = basic_ostring_ref_uwvm_tlc<char16_t>;
        using u32ostring_ref_uwvm_tlc = basic_ostring_ref_uwvm_tlc<char32_t>;
    }  // namespace tlc

    /// @brief vector
    template <::std::movable T, typename Alloc = ::fast_io::native_global_allocator>
    using vector = ::fast_io::containers::vector<T, Alloc>;

    namespace tlc
    {
        template <::std::movable T, typename Alloc = ::fast_io::native_thread_local_allocator>
        using vector = ::uwvm2::utils::container::vector<T, Alloc>;
    }

    /// @brief deque
    template <typename T, typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<T>>
    using deque = ::bizwen::deque<T, Alloc>;

    namespace tlc
    {
        template <typename T, typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<T>>
        using deque = ::uwvm2::utils::container::deque<T, Alloc>;
    }

    /// @brief ordered
    template <typename Key, typename Compare = ::std::less<Key>, typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<Key>>
    using set = ::std::set<Key, Compare, Alloc>;  /// @todo replace with btree set

    template <typename Key,
              typename Val,
              typename Compare = ::std::less<Key>,
              typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<::std::pair<Key const, Val>>>
    using map = ::std::map<Key, Val, Compare, Alloc>;  /// @todo replace with btree map

    namespace tlc
    {
        template <typename Key, typename Compare = ::std::less<Key>, typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<Key>>
        using set = ::uwvm2::utils::container::set<Key, Compare, Alloc>;

        template <typename Key,
                  typename Val,
                  typename Compare = ::std::less<Key>,
                  typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<::std::pair<Key const, Val>>>
        using map = ::uwvm2::utils::container::map<Key, Val, Compare, Alloc>;
    }  // namespace tlc

    /// @brief unordered
    template <typename Key,
              typename Hash = ::std::hash<Key>,
              typename Pred = ::std::equal_to<Key>,
              typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<Key>>
    using unordered_flat_set = ::boost::unordered::unordered_flat_set<Key, Hash, Pred, Alloc>;

    template <typename Key,
              typename Val,
              typename Hash = ::std::hash<Key>,
              typename Pred = ::std::equal_to<Key>,
              typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<::std::pair<Key const, Val>>>
    using unordered_flat_map = ::boost::unordered::unordered_flat_map<Key, Val, Hash, Pred, Alloc>;

    template <typename Key,
              typename Hash = ::std::hash<Key>,
              typename Pred = ::std::equal_to<Key>,
              typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<Key>>
    using unordered_node_set = ::boost::unordered::unordered_node_set<Key, Hash, Pred, Alloc>;

    template <typename Key,
              typename Val,
              typename Hash = ::std::hash<Key>,
              typename Pred = ::std::equal_to<Key>,
              typename Alloc = ::uwvm2::utils::container::fast_io_global_std_allocator<::std::pair<Key const, Val>>>
    using unordered_node_map = ::boost::unordered::unordered_node_map<Key, Val, Hash, Pred, Alloc>;

    namespace tlc
    {
        template <typename Key,
                  typename Hash = ::std::hash<Key>,
                  typename Pred = ::std::equal_to<Key>,
                  typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<Key>>
        using unordered_flat_set = ::uwvm2::utils::container::unordered_flat_set<Key, Hash, Pred, Alloc>;

        template <typename Key,
                  typename Val,
                  typename Hash = ::std::hash<Key>,
                  typename Pred = ::std::equal_to<Key>,
                  typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<::std::pair<Key const, Val>>>
        using unordered_flat_map = ::uwvm2::utils::container::unordered_flat_map<Key, Val, Hash, Pred, Alloc>;

        template <typename Key,
                  typename Hash = ::std::hash<Key>,
                  typename Pred = ::std::equal_to<Key>,
                  typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<Key>>
        using unordered_node_set = ::uwvm2::utils::container::unordered_node_set<Key, Hash, Pred, Alloc>;

        template <typename Key,
                  typename Val,
                  typename Hash = ::std::hash<Key>,
                  typename Pred = ::std::equal_to<Key>,
                  typename Alloc = ::uwvm2::utils::container::fast_io_thread_local_std_allocator<::std::pair<Key const, Val>>>
        using unordered_node_map = ::uwvm2::utils::container::unordered_node_map<Key, Val, Hash, Pred, Alloc>;
    }  // namespace tlc
}

UWVM_MODULE_EXPORT namespace std
{
    template <::std::integral char_type>
    struct hash<::uwvm2::utils::container::basic_string_view<char_type>>
    {
        inline constexpr ::std::size_t operator() (::uwvm2::utils::container::basic_string_view<char_type> obj) const noexcept
        {
#if CHAR_BIT == 8
            ::std::size_t sz;
            if constexpr(requires { obj.size_bytes(); }) { sz = obj.size_bytes(); }
            else
            {
                sz = obj.size() * sizeof(char_type);
            }
            return static_cast<::std::size_t>(::uwvm2::utils::hash::xxh3_64bits(reinterpret_cast<::std::byte const*>(obj.data()), sz));
#else
            // use std hash
            using strvw = ::std::basic_string_view<char_type>;
            return ::std::hash<strvw>{}(strvw{obj});
#endif
        }
    };

    template <::std::integral char_type>
    struct hash<::uwvm2::utils::container::basic_cstring_view<char_type>>
    {
        inline constexpr ::std::size_t operator() (::uwvm2::utils::container::basic_cstring_view<char_type> obj) const noexcept
        {
#if CHAR_BIT == 8
            ::std::size_t sz;
            if constexpr(requires { obj.size_bytes(); }) { sz = obj.size_bytes(); }
            else
            {
                sz = obj.size() * sizeof(char_type);
            }
            return static_cast<::std::size_t>(::uwvm2::utils::hash::xxh3_64bits(reinterpret_cast<::std::byte const*>(obj.data()), sz));
#else
            // use std hash
            using strvw = ::std::basic_string_view<char_type>;
            return ::std::hash<strvw>{}(strvw{obj});
#endif
        }
    };

    template <::std::integral char_type, typename Alloc>
    struct hash<::uwvm2::utils::container::basic_string<char_type, Alloc>>
    {
        inline constexpr ::std::size_t operator() (::uwvm2::utils::container::basic_string<char_type, Alloc> const& obj) const noexcept
        {
#if CHAR_BIT == 8
            ::std::size_t sz;
            if constexpr(requires { obj.size_bytes(); }) { sz = obj.size_bytes(); }
            else
            {
                sz = obj.size() * sizeof(char_type);
            }
            return static_cast<::std::size_t>(::uwvm2::utils::hash::xxh3_64bits(reinterpret_cast<::std::byte const*>(obj.data()), sz));
#else
            // use std hash
            using strvw = ::std::basic_string_view<char_type>;
            return ::std::hash<strvw>{}(strvw{obj});
#endif
        }
    };
}
