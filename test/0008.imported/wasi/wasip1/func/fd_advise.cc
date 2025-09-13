/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      GPT-5
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

#include <cstddef>
#include <cstdint>

#include <uwvm2/imported/wasi/wasip1/func/fd_advise.h>

int main()
{
    using ::uwvm2::imported::wasi::wasip1::abi::advice_t;
    using ::uwvm2::imported::wasi::wasip1::abi::errno_t;
    using ::uwvm2::imported::wasi::wasip1::abi::filesize_t;
    using ::uwvm2::imported::wasi::wasip1::abi::rights_t;
    using ::uwvm2::imported::wasi::wasip1::abi::wasi_posix_fd_t;
    using ::uwvm2::imported::wasi::wasip1::environment::wasip1_environment;
    using ::uwvm2::object::memory::linear::native_memory_t;

    native_memory_t memory{};
    memory.init_by_page_count(1uz);

    wasip1_environment<native_memory_t> env{.wasip1_memory = memory, .argv = {}, .envs = {}, .fd_storage = {}, .trace_wasip1_call = false};

    // Prepare fd table: ensure indices [0..4] exist with valid entries
    env.fd_storage.opens.clear_destroy();
    for(std::size_t i{}; i < 5uz; ++i) { env.fd_storage.opens.emplace_back(); }

    // Case 1: success with valid rights (default constructed rights_base == all)
    {
        // ensure target fd has a valid native handle
        env.fd_storage.opens.index_unchecked(3uz).fd_p->file_fd = ::fast_io::posix_file{u8"test.log", ::fast_io::open_mode::out};
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_advise(
            env,
            static_cast<wasi_posix_fd_t>(3),
            static_cast<filesize_t>(0),
            static_cast<filesize_t>(0),
            advice_t::advice_normal);
        if(ret != errno_t::esuccess)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_advise: expected esuccess for valid rights");
            ::fast_io::fast_terminate();
        }
    }

    // Case 2: enotcapable when rights do not include right_fd_advise
    {
        env.fd_storage.opens.index_unchecked(4uz).fd_p->rights_base = static_cast<rights_t>(0);
        // ensure the target fd has a valid native handle to avoid platform traps on fadvise
        env.fd_storage.opens.index_unchecked(4uz).fd_p->file_fd = ::fast_io::posix_file{u8"test.log", ::fast_io::open_mode::out};

        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_advise(
            env,
            static_cast<wasi_posix_fd_t>(4),
            static_cast<filesize_t>(0),
            static_cast<filesize_t>(0),
            advice_t::advice_normal);
        if(ret != errno_t::enotcapable)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_advise: expected enotcapable when rights missing");
            ::fast_io::fast_terminate();
        }
    }

    // Case 3: ebadf for negative fd
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_advise(
            env,
            static_cast<wasi_posix_fd_t>(-1),
            static_cast<filesize_t>(0),
            static_cast<filesize_t>(0),
            advice_t::advice_normal);
        if(ret != errno_t::ebadf)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_advise: expected ebadf for negative fd");
            ::fast_io::fast_terminate();
        }
    }

    // Case 4: einval for invalid advice enum
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_advise(
            env,
            static_cast<wasi_posix_fd_t>(3),
            static_cast<filesize_t>(0),
            static_cast<filesize_t>(0),
            static_cast<advice_t>(99));
        if(ret != errno_t::einval)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_advise: expected einval for invalid advice");
            ::fast_io::fast_terminate();
        }
    }
}


