
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
# include <climits>
# include <cstring>
# include <limits>
# include <concepts>
# include <bit>
# include <memory>
# include <type_traits>
// macro
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_push_macro.h>
# include <uwvm2/utils/macro/push_macros.h>
// platform
# if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)
#  include <fcntl.h>
#  include <sys/stat.h>
# endif
// import
# include <fast_io.h>
# include <uwvm2/uwvm_predefine/utils/ansies/impl.h>
# include <uwvm2/uwvm_predefine/io/impl.h>
# include <uwvm2/utils/mutex/impl.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/object/memory/linear/impl.h>
# include <uwvm2/imported/wasi/wasip1/abi/impl.h>
# include <uwvm2/imported/wasi/wasip1/fd_manager/impl.h>
# include <uwvm2/imported/wasi/wasip1/memory/impl.h>
# include <uwvm2/imported/wasi/wasip1/environment/impl.h>
# include "base.h"
# include "posix.h"
#endif

#ifndef UWVM_CPP_EXCEPTIONS
# warning "Without enabling C++ exceptions, using this WASI function may cause termination."
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::func
{
    /// @brief     WasiPreview1.fd_advise
    /// @details   __wasi_errno_t fd_advise( __wasi_fd_t fd, __wasi_filesize_t offset, __wasi_filesize_t len, __wasi_advice_t advice);
    /// @note      This function only writes sizes; callers must provide valid memory offsets.

    ::uwvm2::imported::wasi::wasip1::abi::errno_t fd_allocate(
        ::uwvm2::imported::wasi::wasip1::environment::wasip1_environment<::uwvm2::object::memory::linear::native_memory_t> & env,
        ::uwvm2::imported::wasi::wasip1::abi::wasi_posix_fd_t fd,
        [[maybe_unused]] ::uwvm2::imported::wasi::wasip1::abi::filesize_t offset,
        [[maybe_unused]] ::uwvm2::imported::wasi::wasip1::abi::filesize_t len) noexcept
    {
        auto const trace_wasip1_call{env.trace_wasip1_call};

        if(trace_wasip1_call) [[unlikely]]
        {
#ifdef UWVM
            ::fast_io::io::perr(::uwvm2::uwvm::io::u8log_output,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_GREEN),
                                u8"[info]  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"wasip1: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"fd_allocate ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_ORANGE),
                                u8"(wasi-trace)\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
#else
            ::fast_io::io::perr(::fast_io::u8err(), u8"uwvm: [info]  wasip1: fd_allocate (wasi-trace)\n");
#endif
        }

        // The negative value fd is invalid, and this check prevents subsequent undefined behavior.
        if(fd < 0) [[unlikely]] { return ::uwvm2::imported::wasi::wasip1::abi::errno_t::ebadf; }

        auto& wasm_fd_storage{env.fd_storage};

        // The pointer to `wasm_fd` is fixed and remains unchanged even when the vector within `fd_manager` is resized.
        ::uwvm2::imported::wasi::wasip1::fd_manager::wasi_fd_t* curr_wasi_fd_t_p;  // no initialize

        // Subsequent operations involving the file descriptor require locking. curr_fd_release_guard release when return.
        ::uwvm2::utils::mutex::mutex_merely_release_guard_t curr_fd_release_guard{};

        {
            // Prevent operations to obtain the size or perform resizing at this time.
            // Only a lock is required when acquiring the unique pointer for the file descriptor. The lock can be released once the acquisition is complete.
            // Since the file descriptor's location is fixed and accessed via the unique pointer,

            ::uwvm2::utils::mutex::mutex_guard_t fds_lock{wasm_fd_storage.fds_mutex};

            // Negative states have been excluded, so the conversion result will only be positive numbers.
            using unsigned_fd_t = ::std::make_unsigned_t<::uwvm2::imported::wasi::wasip1::abi::wasi_posix_fd_t>;
            auto const unsigned_fd{static_cast<unsigned_fd_t>(fd)};

            // On platforms where `size_t` is smaller than the `fd` type, this check must be added.
            constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
            if constexpr(::std::numeric_limits<unsigned_fd_t>::max() > size_t_max)
            {
                if(unsigned_fd > size_t_max) [[unlikely]] { return ::uwvm2::imported::wasi::wasip1::abi::errno_t::ebadf; }
            }

            auto const fd_opens_pos{static_cast<::std::size_t>(unsigned_fd)};

            // The minimum value in rename_map is greater than opensize.
            if(wasm_fd_storage.opens.size() <= fd_opens_pos)
            {
                // Possibly within the tree being renumbered
                if(auto const renumber_map_iter{wasm_fd_storage.renumber_map.find(fd)}; renumber_map_iter != wasm_fd_storage.renumber_map.end())
                {
                    curr_wasi_fd_t_p = renumber_map_iter->second.fd_p;
                }
                else [[unlikely]]
                {
                    return ::uwvm2::imported::wasi::wasip1::abi::errno_t::ebadf;
                }
            }
            else
            {
                // The addition here is safe.
                curr_wasi_fd_t_p = wasm_fd_storage.opens.index_unchecked(fd_opens_pos).fd_p;
            }

            // curr_wasi_fd_t_p never nullptr
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
            if(curr_wasi_fd_t_p == nullptr) [[unlikely]]
            {
                // Security issues inherent to virtual machines
                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
            }
#endif

            // Other threads will definitely lock fds_mutex when performing close operations (since they need to access the fd vector). If the current thread is
            // performing fadvise, no other thread can be executing any close operations simultaneously, eliminating any destruction issues. Therefore,
            // acquiring the lock at this point is safe. However, the problem arises when, immediately after acquiring the lock and before releasing the manager
            // lock and beginning fd operations, another thread executes a deletion that removes this fd. Subsequent operations by the current thread would then
            // encounter issues. Thus, locking must occur before releasing fds_mutex.
            curr_fd_release_guard.device_p = ::std::addressof(curr_wasi_fd_t_p->fd_mutex);
            curr_fd_release_guard.lock();

            // After unlocking fds_lock, members within `wasm_fd_storage_t` can no longer be accessed or modified.
        }

        // curr_fd_uniptr is not null.
        auto& curr_fd{*curr_wasi_fd_t_p};

        if((curr_fd.rights_base & ::uwvm2::imported::wasi::wasip1::abi::rights_t::right_fd_allocate) !=
           ::uwvm2::imported::wasi::wasip1::abi::rights_t::right_fd_allocate) [[unlikely]]
        {
            return ::uwvm2::imported::wasi::wasip1::abi::errno_t::enotcapable;
        }

#if defined(__APPLE__) || defined(__DARWIN_C_LEVEL)

        auto const curr_fd_native_handle{curr_fd.file_fd.native_handle()};

        using underlying_filesize_t = ::std::underlying_type_t<::uwvm2::imported::wasi::wasip1::abi::filesize_t>;

        off_t fallocate_offset;  // no initialize
        off_t fallocate_len;     // no initialize

        // Saturation treatment
        if constexpr(::std::numeric_limits<underlying_filesize_t>::max() > ::std::numeric_limits<off_t>::max())
        {
            if(static_cast<underlying_filesize_t>(offset) > ::std::numeric_limits<off_t>::max()) [[unlikely]]
            {
                fallocate_offset = ::std::numeric_limits<off_t>::max();
            }
            else
            {
                fallocate_offset = static_cast<off_t>(offset);
            }

            if(static_cast<underlying_filesize_t>(len) > ::std::numeric_limits<off_t>::max()) [[unlikely]]
            {
                fallocate_len = ::std::numeric_limits<off_t>::max();
            }
            else
            {
                fallocate_len = static_cast<off_t>(len);
            }
        }
        else
        {
            fallocate_offset = static_cast<off_t>(offset);
            fallocate_len = static_cast<off_t>(len);
        }

        // macOS: use F_PREALLOCATE / fstore_t
        fstore_t fstore{.fst_flags = F_ALLOCATECONTIG,
                        .fst_posmode = F_VOLPOSMODE,  // the offset parameter in WASI fd_allocate is calculated from the beginning of the file.
                        .fst_offset = fallocate_offset,
                        .fst_length = fallocate_len,
                        .fst_bytesalloc = 0u};

        off_t newsize;  // no initialize
        if(::uwvm2::imported::wasi::wasip1::func::add_overflow(fallocate_offset, fallocate_len, newsize)) [[unlikely]]
        {
            return ::uwvm2::imported::wasi::wasip1::abi::errno_t::efbig;
        }

        if(::uwvm2::imported::wasi::wasip1::func::posix::fcntl(curr_fd_native_handle, F_PREALLOCATE, ::std::addressof(fstore)) == -1) [[unlikely]]
        {
            // try non-contiguous allocation
            fstore.fst_flags = F_ALLOCATEALL;
            if(::uwvm2::imported::wasi::wasip1::func::posix::fcntl(curr_fd_native_handle, F_PREALLOCATE, ::std::addressof(fstore)) == -1) [[unlikely]]
            {
                // fallback: ftruncate to ensure file is large enough (may not preallocate underlying blocks)

                if(::uwvm2::imported::wasi::wasip1::func::posix::ftruncate(curr_fd_native_handle, newsize) == -1) [[unlikely]]
                {
                    return ::uwvm2::imported::wasi::wasip1::abi::errno_t::einval;
                }
            }
        }

        // After successful preallocate, ensure file size covers the region
        struct stat st;
        if(::uwvm2::imported::wasi::wasip1::func::posix::fstat(curr_fd_native_handle, ::std::addressof(st)) == -1) [[unlikely]]
        {
            return ::uwvm2::imported::wasi::wasip1::abi::errno_t::einval;
        }

        if(st.st_size < newsize) [[unlikely]]
        {
            if(::uwvm2::imported::wasi::wasip1::func::posix::ftruncate(curr_fd_native_handle, newsize) == -1) [[unlikely]]
            {
                return ::uwvm2::imported::wasi::wasip1::abi::errno_t::einval;
            }
        }

#elif defined(_WIN32) || defined(__CYGWIN__)
        // Windows path. We accept a C runtime descriptor. Convert to HANDLE.
        auto const& curr_posix_file {curr_fd.file_fd};
        auto const curr_win32_io_observer{static_cast<::fast_io::win32_io_observer>(curr_posix_file)};
        auto const curr_fd_native_handle{curr_win32_io_observer.native_handle()};

        // Attempt to set allocation size using SetFileInformationByHandle (FileAllocationInfo)
        // FILE_ALLOCATION_INFO contains AllocationSize (LARGE_INTEGER)
        typedef struct _FILE_ALLOCATION_INFO_WIN
        {
            LARGE_INTEGER AllocationSize;
        } FILE_ALLOCATION_INFO_WIN;

        FILE_ALLOCATION_INFO_WIN faInfo;
        faInfo.AllocationSize.QuadPart = (LONGLONG)(offset + len);

        if(SetFileInformationByHandle(h, FileAllocationInfo, &faInfo, sizeof(faInfo)))
        {
            // Optionally ensure file logical size covers region
            LARGE_INTEGER cur;
            if(!GetFileSizeEx(h, &cur)) { return EIO; }
            if(cur.QuadPart < (LONGLONG)(offset + len))
            {
                // Move file pointer and set end of file
                LARGE_INTEGER move;
                move.QuadPart = offset + len;
                if(!SetFilePointerEx(h, move, NULL, FILE_BEGIN)) { return EIO; }
                if(!SetEndOfFile(h)) { return EIO; }
            }
            return 0;
        }

        // Fallbacks for older Windows (or if operation not permitted)
        // Try SetEndOfFile via MoveFilePointer
        {
            LARGE_INTEGER cur;
            if(!GetFileSizeEx(h, &cur)) { return EIO; }
            if(cur.QuadPart < (LONGLONG)(offset + len))
            {
                LARGE_INTEGER move;
                move.QuadPart = offset + len;
                if(!SetFilePointerEx(h, move, NULL, FILE_BEGIN)) { return EIO; }
                if(!SetEndOfFile(h)) { return EIO; }
            }
            return 0;
        }

#elif (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(_WIN32) || defined(__CYGWIN__)) &&             \
    __has_include(<dirent.h>) && !defined(_PICOLIBC__)
        // Prefer posix_fallocate (portable) -- it returns 0 on success or an errno value on failure.
        auto const curr_fd_native_handle{curr_fd.file_fd.native_handle()};

        // posix_fallocate signature: int posix_fallocate(int fd, off_t offset, off_t len);
        // But on some platforms off_t might be smaller; do saturation.
        using underlying_filesize_t = ::std::underlying_type_t<::uwvm2::imported::wasi::wasip1::abi::filesize_t>;

        off_t fallocate_offset;  // no initialize
        off_t fallocate_len;     // no initialize

        // Saturation treatment
        if constexpr(::std::numeric_limits<underlying_filesize_t>::max() > ::std::numeric_limits<off_t>::max())
        {
            if(static_cast<underlying_filesize_t>(offset) > ::std::numeric_limits<off_t>::max()) [[unlikely]]
            {
                fallocate_offset = ::std::numeric_limits<off_t>::max();
            }
            else
            {
                fallocate_offset = static_cast<off_t>(offset);
            }

            if(static_cast<underlying_filesize_t>(len) > ::std::numeric_limits<off_t>::max()) [[unlikely]]
            {
                fallocate_len = ::std::numeric_limits<off_t>::max();
            }
            else
            {
                fallocate_len = static_cast<off_t>(len);
            }
        }
        else
        {
            fallocate_offset = static_cast<off_t>(offset);
            fallocate_len = static_cast<off_t>(len);
        }

# if defined(__linux__) && defined(__NR_fallocate)
        int result{::fast_io::system_call<__NR_fallocate, int>(curr_fd_native_handle, 0, fallocate_offset, fallocate_len)};
        if(result == -1) [[unlikely]] { return ::uwvm2::imported::wasi::wasip1::abi::errno_t::einval; };
# else
        int result{::uwvm2::imported::wasi::wasip1::func::posix::posix_fallocate(curr_fd_native_handle, fallocate_offset, fallocate_len)};
        if(result == -1) [[unlikely]] { return ::uwvm2::imported::wasi::wasip1::abi::errno_t::einval; };
# endif
#endif

        return ::uwvm2::imported::wasi::wasip1::abi::errno_t::esuccess;
    }
}  // namespace uwvm2::imported::wasi::wasip1::func

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_pop_macro.h>
#endif

