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

#include <uwvm2/imported/wasi/wasip1/func/fd_fdstat_set_flags.h>
#include <uwvm2/imported/wasi/wasip1/func/fd_fdstat_get.h>
#include <uwvm2/imported/wasi/wasip1/func/fd_close.h>
#include <uwvm2/imported/wasi/wasip1/func/posix.h>

#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)

int main()
{
    using ::uwvm2::imported::wasi::wasip1::abi::errno_t;
    using ::uwvm2::imported::wasi::wasip1::abi::fdflags_t;
    using ::uwvm2::imported::wasi::wasip1::abi::rights_t;
    using ::uwvm2::imported::wasi::wasip1::abi::wasi_posix_fd_t;
    using ::uwvm2::imported::wasi::wasip1::environment::wasip1_environment;
    using ::uwvm2::object::memory::linear::native_memory_t;

    native_memory_t memory{};
    memory.init_by_page_count(1uz);

    wasip1_environment<native_memory_t> env{.wasip1_memory = memory, .argv = {}, .envs = {}, .fd_storage = {}, .trace_wasip1_call = false};

    // Prepare fd table
    env.fd_storage.opens.resize(6uz);

    // Negative fd => ebadf
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env, static_cast<wasi_posix_fd_t>(-1), static_cast<fdflags_t>(0));
        if(ret != errno_t::ebadf)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: expected ebadf for negative fd");
            ::fast_io::fast_terminate();
        }
    }

    // Setup fd=4 with valid file and all rights
    auto& fd4 = *env.fd_storage.opens.index_unchecked(4uz).fd_p;
    fd4.file_fd = ::fast_io::native_file{u8"test_fd_fdstat_set_flags.tmp", ::fast_io::open_mode::out | ::fast_io::open_mode::creat | ::fast_io::open_mode::trunc};
    fd4.rights_base = static_cast<rights_t>(-1);
    fd4.rights_inherit = static_cast<rights_t>(-1);

    int const native_fd = fd4.file_fd.native_handle();

    // Baseline: ensure we can read current flags
    int const curr0 = ::uwvm2::imported::wasi::wasip1::func::posix::fcntl(native_fd, F_GETFL);
    if(curr0 == -1)
    {
        ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: F_GETFL failed");
        ::fast_io::fast_terminate();
    }

    // Case 1: set only APPEND, verify APPEND is set and other managed bits cleared
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env,
                                                                                    static_cast<wasi_posix_fd_t>(4),
                                                                                    fdflags_t::fdflag_append);
        if(ret != errno_t::esuccess)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: set append expected esuccess");
            ::fast_io::fast_terminate();
        }

        int const flags1 = ::uwvm2::imported::wasi::wasip1::func::posix::fcntl(native_fd, F_GETFL);
        if(flags1 == -1)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: F_GETFL after set failed");
            ::fast_io::fast_terminate();
        }

# ifdef O_APPEND
        if((flags1 & O_APPEND) == 0)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: O_APPEND not set after call");
            ::fast_io::fast_terminate();
        }
# endif
# ifdef O_NONBLOCK
        if((flags1 & O_NONBLOCK) != 0)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: O_NONBLOCK should be cleared when not requested");
            ::fast_io::fast_terminate();
        }
# endif
    }

    // Case 2: clear APPEND (pass 0), verify APPEND cleared
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env, static_cast<wasi_posix_fd_t>(4), static_cast<fdflags_t>(0));
        if(ret != errno_t::esuccess)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: clear append expected esuccess");
            ::fast_io::fast_terminate();
        }

        int const flags2 = ::uwvm2::imported::wasi::wasip1::func::posix::fcntl(native_fd, F_GETFL);
        if(flags2 == -1)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: F_GETFL after clear failed");
            ::fast_io::fast_terminate();
        }
# ifdef O_APPEND
        if((flags2 & O_APPEND) != 0)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: O_APPEND should be cleared");
            ::fast_io::fast_terminate();
        }
# endif
    }

    // Case 3: set NONBLOCK through WASI and verify
    {
        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env,
                                                                                    static_cast<wasi_posix_fd_t>(4),
                                                                                    fdflags_t::fdflag_nonblock);
        if(ret != errno_t::esuccess)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: set nonblock expected esuccess");
            ::fast_io::fast_terminate();
        }

        int const flags3 = ::uwvm2::imported::wasi::wasip1::func::posix::fcntl(native_fd, F_GETFL);
        if(flags3 == -1)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: F_GETFL after nonblock failed");
            ::fast_io::fast_terminate();
        }
# ifdef O_NONBLOCK
        if((flags3 & O_NONBLOCK) == 0)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: O_NONBLOCK not set");
            ::fast_io::fast_terminate();
        }
# endif
    }

    // Case 4: enotcapable when rights missing
    {
        auto& fde = *env.fd_storage.opens.index_unchecked(3uz).fd_p;
        fde.file_fd = ::fast_io::native_file{u8"test_fd_fdstat_set_flags_rights.tmp",
                                             ::fast_io::open_mode::out | ::fast_io::open_mode::creat | ::fast_io::open_mode::trunc};
        fde.rights_base = static_cast<rights_t>(0);

        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env,
                                                                                    static_cast<wasi_posix_fd_t>(3),
                                                                                    fdflags_t::fdflag_append);
        if(ret != errno_t::enotcapable)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: expected enotcapable when rights missing");
            ::fast_io::fast_terminate();
        }
    }

    // Case 5: ebadf after fd_close
    {
        auto& fde = *env.fd_storage.opens.index_unchecked(2uz).fd_p;
        fde.file_fd = ::fast_io::native_file{u8"test_fd_fdstat_set_flags_close.tmp",
                                             ::fast_io::open_mode::out | ::fast_io::open_mode::creat | ::fast_io::open_mode::trunc};
        fde.rights_base = static_cast<rights_t>(-1);

        auto const closed = ::uwvm2::imported::wasi::wasip1::func::fd_close(env, static_cast<wasi_posix_fd_t>(2));
        if(closed != errno_t::esuccess)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: close expected esuccess");
            ::fast_io::fast_terminate();
        }

        auto const ret = ::uwvm2::imported::wasi::wasip1::func::fd_fdstat_set_flags(env,
                                                                                    static_cast<wasi_posix_fd_t>(2),
                                                                                    fdflags_t::fdflag_append);
        if(ret != errno_t::ebadf)
        {
            ::fast_io::io::perrln(::fast_io::u8err(), u8"fd_fdstat_set_flags: expected ebadf after fd_close");
            ::fast_io::fast_terminate();
        }
    }
}

#else

int main() {}

#endif


