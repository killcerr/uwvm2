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
        ::fast_io::vector<::uwvm2::imported::wasi::wasip1::fd_manager::wasm_fd> opens{};
        ::fast_io::vector<::std::size_t> closes{};
        ::fast_io::native_mutex fds_mutex{};  // [singleton]
        ::std::size_t fd_limit{};
    };

    inline void init(wasm_fd_storage_t & wasm_fd_storage, ::std::size_t fd_limit) noexcept
    {
        // Initialization is done in the main thread, with invalid locks.
        wasm_fd_storage.fd_limit = fd_limit;
        wasm_fd_storage.opens.reserve(fd_limit);
        wasm_fd_storage.closes.reserve(fd_limit);
    }

}  // namespace uwvm2::parser::wasm::base

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
