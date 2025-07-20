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
# include <stdfloat>
# include <limits>
# include <type_traits>
# include <memory>
# include <new>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/vector.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/imported/wasi/wasip1/abi/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::fd_manager
{
    struct wasi_fd_t
    {
        using mutex_t = ::fast_io::native_mutex;
        using mutex_alloc_t = ::fast_io::native_typed_global_allocator<mutex_t>;

        // ====== for wasi ======
        ::fast_io::posix_file file_fd{};
#if defined(_WIN32) && defined(_WIN32_WINDOWS)
        ::fast_io::win32_9xa_dir_file dir_fd{};
        bool is_dir{};
#endif
        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_base{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};
        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_inherit{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};

        // ====== for vm ======
        mutex_t fd_mutex{};  // [singleton]
        ::std::size_t close_pos{SIZE_MAX};

        inline constexpr wasi_fd_t() noexcept = default;

        inline constexpr wasi_fd_t(wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t& operator= (wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t(wasi_fd_t&& other) noexcept = delete;

        inline constexpr wasi_fd_t& operator= (wasi_fd_t&& other) noexcept = delete;

        inline constexpr ~wasi_fd_t() = default;
    };

    struct wasi_fd_unique_ptr_t
    {
        using wasi_fd_t_fast_io_type_allocator = ::fast_io::native_typed_global_allocator<wasi_fd_t>;

        wasi_fd_t* fd_p;

        inline constexpr wasi_fd_unique_ptr_t() noexcept
        {
            fd_p = wasi_fd_t_fast_io_type_allocator::allocate(1uz);
            ::new(fd_p) wasi_fd_t{};
        }

        inline constexpr wasi_fd_unique_ptr_t(wasi_fd_unique_ptr_t const&) noexcept = delete;
        inline constexpr wasi_fd_unique_ptr_t& operator= (wasi_fd_unique_ptr_t const&) noexcept = delete;

        inline constexpr wasi_fd_unique_ptr_t(wasi_fd_unique_ptr_t&& other) noexcept
        {
            this->fd_p = other.fd_p;
            other.fd_p = nullptr
        }

        inline constexpr wasi_fd_unique_ptr_t& operator= (wasi_fd_unique_ptr_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            ::std::destroy_at(fd_p);
            wasi_fd_t_fast_io_type_allocator::deallocate_n(fd_p, 1uz);

            this->fd_p = other.fd_p;
            other.fd_p = nullptr
        }

        inline constexpr ~wasi_fd_unique_ptr_t()
        {
            ::std::destroy_at(fd_p);
            wasi_fd_t_fast_io_type_allocator::deallocate_n(fd_p, 1uz);
        }
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
