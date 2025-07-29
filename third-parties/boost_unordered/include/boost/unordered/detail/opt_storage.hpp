// Copyright (C) 2023 Christian Mazakas
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_OPT_STORAGE_HPP
#define BOOST_UNORDERED_DETAIL_OPT_STORAGE_HPP

#include "../../minconfig.hpp"

#include <memory>

namespace boost {
  namespace unordered {
    namespace detail {
      template <class T> union opt_storage
      {
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	      [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	      [[no_unique_address]]
#endif
#endif
        T t_;

        opt_storage() {}
        ~opt_storage() {}

        T* address() noexcept { return std::addressof(t_); }
        T const* address() const noexcept { return std::addressof(t_); }
      };
    } // namespace detail
  } // namespace unordered
} // namespace boost

#endif // BOOST_UNORDERED_DETAIL_OPT_STORAGE_HPP
