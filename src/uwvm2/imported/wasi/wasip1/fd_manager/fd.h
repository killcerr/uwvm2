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

#ifdef UWVM_MODULE
import fast_io;
import uwvm2.parser.wasm.standard.wasm1.type import uwvm2.imported.wasi.wasip1.abi;
#else
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
#if defined(_WIN32) && defined(_WIN32_WINDOWS)
    struct wasi_fd_t
    {
        using mutex_t = ::fast_io::native_mutex;
        using mutex_alloc_t = ::fast_io::native_typed_global_allocator<mutex_t>;

        // ====== for wasi ======
        ::fast_io::posix_file file_fd{};
        ::fast_io::win32_9xa_dir_file dir_fd{};
        bool is_dir{};

        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_base{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};
        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_inherit{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};

        // ====== for vm ======
        mutex_t* fd_mutex{};
        ::std::size_t close_pos{SIZE_MAX};

        inline constexpr wasi_fd_t() noexcept
        {
            this->fd_mutex = mutex_alloc_t::allocate(1uz);
            ::new(this->fd_mutex) mutex_t{};
        }

        inline constexpr wasi_fd_t(wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t& operator= (wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t(wasi_fd_t&& other) noexcept
        {
            // Prevents the vector managed behind the scenes from expanding while in use, requiring locks to be added
            ::fast_io::io_lock_guard fd_lock{*other.fd_mutex};

            ::new(::std::addressof(this->file_fd)) decltype(this->file_fd){::std::move(other.file_fd)};
            ::new(::std::addressof(this->win9x_dir_file)) decltype(this->win9x_dir_file){::std::move(other.win9x_dir_file)};

            this->is_dir = other.is_dir;
            this->rights_base = other.rights_base;
            this->rights_inherit = other.rights_inherit;
            this->fd_mutex = other.fd_mutex;
            this->close_pos = other.close_pos;

            other.is_dir = false;
            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.fd_mutex = nullptr;
            other.close_pos = SIZE_MAX;
        }

        inline constexpr wasi_fd_t& operator= (wasi_fd_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // Prevents the vector managed behind the scenes from expanding while in use, requiring locks to be added
            ::fast_io::io_lock_guard fd_lock{*other.fd_mutex};

            this->file_fd = ::std::move(other.file_fd);
            this->win9x_dir_file = ::std::move(other.win9x_dir_file);

            this->is_dir = other.is_dir;
            this->rights_base = other.rights_base;
            this->rights_inherit = other.rights_inherit;
            this->fd_mutex = other.fd_mutex;
            this->close_pos = other.close_pos;

            other.is_dir = false;
            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.fd_mutex = nullptr;
            other.close_pos = SIZE_MAX;

            return *this;
        }

        ~wasi_fd_t()
        {
            // Multiple calls to the destructor are ub, so no type changes are made here
            ::std::destroy_at(this->fd_mutex);
            mutex_alloc_t::deallocate_n(this->fd_mutex, 1uz);
        }

        void close()
        {
            ::fast_io::io_lock_guard fd_look{*(this->fd_mutex)};
            this->file_fd.close();
            this->dir_fd.close();
            other.is_dir = false;
            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
        }
    };

#else  // winnt, posix, dos

    struct wasi_fd_t
    {
        using mutex_t = ::fast_io::native_mutex;
        using mutex_alloc_t = ::fast_io::native_typed_global_allocator<mutex_t>;

        // ====== for wasi ======
        ::fast_io::posix_file file_fd{};

        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_base{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};
        ::uwvm2::imported::wasi::wasip1::abi::rights_t rights_inherit{static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1)};

        // ====== for vm ======
        mutex_t* fd_mutex{};
        ::std::size_t close_pos{SIZE_MAX};

        inline constexpr wasi_fd_t() noexcept
        {
            this->fd_mutex = mutex_alloc_t::allocate(1uz);
            ::new(this->fd_mutex) mutex_t{};
        }

        inline constexpr wasi_fd_t(wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t& operator= (wasi_fd_t const& other) noexcept = delete;

        inline constexpr wasi_fd_t(wasi_fd_t&& other) noexcept
        {
            // Prevents the vector managed behind the scenes from expanding while in use, requiring locks to be added
            ::fast_io::io_lock_guard fd_lock{*other.fd_mutex};

            ::new(::std::addressof(this->file_fd)) decltype(this->file_fd){::std::move(other.file_fd)};

            this->rights_base = other.rights_base;
            this->rights_inherit = other.rights_inherit;
            this->fd_mutex = other.fd_mutex;
            this->close_pos = other.close_pos;

            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.fd_mutex = nullptr;
            other.close_pos = SIZE_MAX;
        }

        inline constexpr wasi_fd_t& operator= (wasi_fd_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // Prevents the vector managed behind the scenes from expanding while in use, requiring locks to be added
            ::fast_io::io_lock_guard fd_lock{*other.fd_mutex};

            this->file_fd = ::std::move(other.file_fd);

            this->rights_base = other.rights_base;
            this->rights_inherit = other.rights_inherit;
            this->fd_mutex = other.fd_mutex;
            this->close_pos = other.close_pos;

            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.fd_mutex = nullptr;
            other.close_pos = SIZE_MAX;

            return *this;
        }

        ~wasi_fd_t()
        {
            // Multiple calls to the destructor are ub, so no type changes are made here
            ::std::destroy_at(this->fd_mutex);
            mutex_alloc_t::deallocate_n(this->fd_mutex, 1uz);
        }

        void close()
        {
            ::fast_io::io_lock_guard fd_look{*(this->fd_mutex)};
            this->file_fd.close();
            other.rights_base = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
            other.rights_inherit = static_cast<::uwvm2::imported::wasi::wasip1::abi::rights_t>(-1);
        }
    };    
#endif

}  // namespace uwvm2::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
