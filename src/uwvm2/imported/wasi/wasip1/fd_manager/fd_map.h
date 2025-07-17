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
import uwvm2.utils.container;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.imported.wasi.wasip1.abi;
import :fd;
#else
// std
# include <cstddef>
# include <cstdint>
# include <stdfloat>
# include <limits>
# include <type_traits>
# include <memory>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/vector.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/imported/wasi/wasip1/abi/impl.h>
# include "fd.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::fd_manager
{
    /// @brief [singleton]
    struct wasm_fd_storage_t
    {
        // Ensure that iterators can't fail during expansion
        ::uwvm2::utils::container::deque<::uwvm2::imported::wasi::wasip1::fd_manager::wasi_fd_t> opens{};
        // Used to record the coordinates of closure for subsequent builds
        ::fast_io::vector<::std::size_t> closes{};
        ::fast_io::native_mutex fds_mutex{};  // [singleton]
        ::std::size_t fd_limit{};
    };

    inline constexpr void init(wasm_fd_storage_t & wasm_fd_storage, ::std::size_t fd_limit) noexcept
    {
        // Initialization is done in the main thread, with invalid locks.
        wasm_fd_storage.fd_limit = fd_limit;
        auto const reserve_sizes{::std::min(1024uz, fd_limit)};
        wasm_fd_storage.opens.reserve(reserve_sizes);
        wasm_fd_storage.closes.reserve(reserve_sizes);
    }

    inline constexpr ::uwvm::vm::wasi::wasi_fd_t* get_original_wasm_fd_p(
        wasm_fd_storage_t& wasm_fd_storage, ::uwvm2::imported::wasi::wasip1::fd_manager::wasi_posix_fd_t wfd) noexcept
    {
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(wfd > size_t_max) [[unlikely]]
            {
                return nullptr;
            }
        }

        auto const wasm_fd_pos{static_cast<::std::size_t>(wfd)};

        ::fast_io::io_lock_guard fds_lock{wasm_fd_storage.fds_mutex};

        // wasm_fd_storage.opens can't be changed.

        if(wasm_fd_storage.opens.size() <= wasm_fd_pos) [[unlikely]] { return nullptr; }
        else { return wasm_fd_storage.opens.begin() + wasm_fd_pos; }
    }

    struct create_wasm_fd_ret
    {
        ::uwvm2::imported::wasi::wasip1::fd_manager::wasi_posix_fd_t wasi_posix_fd{};
        ::uwvm2::imported::wasi::wasip1::fd_manager::wasi_fd_t* wasi_fd{};
    };

    inline constexpr create_wasm_fd_ret create_wasm_fd(wasm_fd_storage_t& wasm_fd_storage) noexcept
    {
        ::fast_io::io_lock_guard fds_lock{wasm_fd_storage.fds_mutex};

        // wasm_fd_storage.opens can't be changed.

        if(wasm_fd_storage.closes.empty())
        {
            if(wasm_fd_storage.opens.size() == wasm_fd_storage.fd_limit) [[unlikely]] 
            { 
                return {static_cast<::uwvm2::imported::wasi::wasip1::fd_manager::wasi_posix_fd_t>(-1), nullptr}; 
            }

            auto const ret_wasi_fd{::std::addressof(wasm_fd_storage.opens.emplace_back())};
            auto const ret_wasi_posix_fd{static_cast<::uwvm2::imported::wasi::wasip1::fd_manager::wasi_posix_fd_t>(static_cast<::std::size_t>(ret_wasi_fd - wasm_fd_storage.opens.begin()))};
            return {ret_wasi_posix_fd, ret_wasi_fd};
        }
        else
        {
            auto const ret_wasi_posix_fd_uz{wasm_fd_storage.closes.back_unchecked()};
            wasm_fd_storage.closes.pop_back_unchecked();
            auto const ret_wasi_fd{wasm_fd_storage.opens.begin() + ret_wasi_posix_fd_uz};
            auto const ret_wasi_posix_fd{static_cast<::uwvm2::imported::wasi::wasip1::fd_manager::wasi_posix_fd_t>(ret_wasi_posix_fd_uz)};
            return {ret_wasi_posix_fd, ret_wasi_fd};
        }
    }

}  // namespace uwvm2::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
