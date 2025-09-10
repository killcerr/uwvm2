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
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::abi
{
    /// @brief    WASI ABI for WASM64
    /// @details  In the traditional wasip1, the ABI is based on 32-bit pointers (i32 offset into linear memory). When memory64 support was introduced, to
    ///           maintain compatibility with preview1 programs, a set of mirrored APIs was provided, simply appending `_wasm64` to their names. Although some
    ///           programs remain wasm64-wasip1 (32-bit ABI), this does not affect usage. Only portions of pointers exceeding 4GB are truncated. Thus, providing
    ///           a memory64 compatibility mode for wasip1 ensures full compatibility.

    // static assert

    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_i8) == 1uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8) == 1uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_i16) == 2uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16) == 2uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_i32) == 4uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32) == 4uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_i64) == 8uz, "non-wasi data layout");
    static_assert(sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64) == 8uz, "non-wasi data layout");

    // wasm64 pointer-sized types
    using wasi_intptr_wasm64_t = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_i64;
    using wasi_uintptr_wasm64_t = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64;
    using wasi_size_wasm64_t = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64;
    using wasi_void_ptr_wasm64_t = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64;

    using wasi_posix_fd_wasm64_t = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_i32;

    enum class advice_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        advice_normal,
        advice_sequential,
        advice_random,
        advice_willneed,
        advice_dontneed,
        advice_noreuse,

    };

    enum class clockid_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {
        clock_realtime,
        clock_monotonic,
        clock_process_cputime_id,
        clock_thread_cputime_id,

    };

    enum class device_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class dirnamlen_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {

    };

    enum class dircookie_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {
        dircookie_start
    };

    enum class errno_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        esuccess,
        e2big,
        eacces,
        eaddrinuse,
        eaddrnotavail,
        eafnosupport,
        eagain,
        ealready,
        ebadf,
        ebadmsg,
        ebusy,
        ecanceled,
        echild,
        econnaborted,
        econnrefused,
        econnreset,
        edeadlk,
        edestaddrreq,
        edom,
        edquot,
        eexist,
        efault,
        efbig,
        ehostunreach,
        eidrm,
        eilseq,
        einprogress,
        eintr,
        einval,
        eio,
        eisconn,
        eisdir,
        eloop,
        emfile,
        emlink,
        emsgsize,
        emultihop,
        enametoolong,
        enetdown,
        enetreset,
        enetunreach,
        enfile,
        enobufs,
        enodev,
        enoent,
        enoexec,
        enolck,
        enolink,
        enomem,
        enomsg,
        enoprotoopt,
        enospc,
        enosys,
        enotconn,
        enotdir,
        enotempty,
        enotrecoverable,
        enotsock,
        enotsup,
        enotty,
        enxio,
        eoverflow,
        eownerdead,
        eperm,
        epipe,
        eproto,
        eprotonosupport,
        eprototype,
        erange,
        erofs,
        espipe,
        esrch,
        estale,
        etimedout,
        etxtbsy,
        exdev,
        enotcapable,
    };

    enum class eventrwflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        event_fd_readwrite_hangup = 0x0001
    };

    enum class eventtype_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        eventtype_clock,
        eventtype_fd_read,
        eventtype_fd_write,
    };

    enum class exitcode_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {

    };

    enum class fd_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {

    };

    enum class fdflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        fdflag_append = 0x0001,
        fdflag_dsync = 0x0002,
        fdflag_nonblock = 0x0004,
        fdflag_rsync = 0x0008,
        fdflag_sync = 0x0010,
    };

    enum class filedelta_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_i64
    {

    };

    enum class filesize_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class filetype_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        filetype_unknown,
        filetype_block_device,
        filetype_character_device,
        filetype_directory,
        filetype_regular_file,
        filetype_socket_dgram,
        filetype_socket_stream,
        filetype_symbolic_link,
    };

    enum class fstflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        filestat_set_atim = 0x0001,
        filestat_set_atim_now = 0x0002,
        filestat_set_mtim = 0x0004,
        filestat_set_mtim_now = 0x0008,
    };

    enum class inode_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class linkcount_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class lookupflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {
        lookup_symlink_follow = 0x00000001
    };

    enum class oflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        o_creat = 0x0001,
        o_directory = 0x0002,
        o_excl = 0x0004,
        o_trunc = 0x0008,
    };

    enum class riflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        sock_recv_peek = 0x0001,
        sock_recv_waitall = 0x0002,
    };

    enum class rights_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {
        right_fd_datasync = 0x0000000000000001,
        right_fd_read = 0x0000000000000002,
        right_fd_seek = 0x0000000000000004,
        right_fd_fdstat_set_flags = 0x0000000000000008,
        right_fd_sync = 0x0000000000000010,
        right_fd_tell = 0x0000000000000020,
        right_fd_write = 0x0000000000000040,
        right_fd_advise = 0x0000000000000080,
        right_fd_allocate = 0x0000000000000100,
        right_path_create_directory = 0x0000000000000200,
        right_path_create_file = 0x0000000000000400,
        right_path_link_source = 0x0000000000000800,
        right_path_link_target = 0x0000000000001000,
        right_path_open = 0x0000000000002000,
        right_fd_readdir = 0x0000000000004000,
        right_path_readlink = 0x0000000000008000,
        right_path_rename_source = 0x0000000000010000,
        right_path_rename_target = 0x0000000000020000,
        right_path_filestat_get = 0x0000000000040000,
        right_path_filestat_set_size = 0x0000000000080000,
        right_path_filestat_set_times = 0x0000000000100000,
        right_fd_filestat_get = 0x0000000000200000,
        right_fd_filestat_set_size = 0x0000000000400000,
        right_fd_filestat_set_times = 0x0000000000800000,
        right_path_symlink = 0x0000000001000000,
        right_path_remove_directory = 0x0000000002000000,
        right_path_unlink_file = 0x0000000004000000,
        right_poll_fd_readwrite = 0x0000000008000000,
        right_sock_shutdown = 0x0000000010000000,
    };

    enum class roflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        sock_recv_data_truncated = 0x0001,
    };

    enum class sdflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        shut_rd = 0x01,
        shut_wr = 0x02,
    };

    enum class siflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {

    };

    enum class signal_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        sighup = 1,
        sigint,
        sigquit,
        sigill,
        sigtrap,
        sigabrt,
        sigbus,
        sigfpe,
        sigkill,
        sigusr1,
        sigsegv,
        sigusr2,
        sigpipe,
        sigalrm,
        sigterm,
        sigchld,
        sigcont,
        sigstop,
        sigtstp,
        sigttin,
        sigttou,
        sigurg,
        sigxcpu,
        sigxfsz,
        sigvtalrm,
        sigprof,
        sigwinch,
        sigpoll,
        sigpwr,
        sigsys,

    };

    enum class subclockflags_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16
    {
        subscription_clock_abstime = 0x0001
    };

    enum class timestamp_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class userdata_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u64
    {

    };

    enum class whence_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        whence_set,
        whence_cur,
        whence_end,

    };

    enum class preopentype_wasm64_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8
    {
        preopentype_dir
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
