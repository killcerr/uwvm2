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
#  include <sys/socket.h>
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
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::func
{
#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)
    namespace posix
    {
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
        extern int posix_fadvise(int fd, ::off_t offset, ::off_t len, int advice) noexcept __asm__("posix_fadvise");
        extern int fallocate(int fd, int mode, ::off_t offset, ::off_t len) noexcept __asm__("fallocate");
        extern int posix_fallocate(int fd, ::off_t offset, ::off_t size) noexcept __asm__("posix_fallocate");
# endif

        extern int fcntl(int fd, int cmd, ... /* arg */) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("fcntl")
# else
            __asm__("_fcntl")
# endif
                ;
        extern int ftruncate(int fd, ::off_t size) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("ftruncate")
# else
            __asm__("_ftruncate")
# endif
                ;
        extern int fstat(int fd, struct stat* st) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("fstat")
# else
            __asm__("_fstat")
# endif
                ;
        extern int fdatasync(int fd) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("fdatasync")
# else
            __asm__("_fdatasync")
# endif
                ;
        extern int fsync(int fd) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("fsync")
# else
            __asm__("_fsync")
# endif
                ;
        extern int getsockopt(int, int, int, void* __restrict, ::socklen_t* __restrict) noexcept
# if !(defined(__MSDOS__) || defined(__DJGPP__)) && !(defined(__APPLE__) || defined(__DARWIN_C_LEVEL))
            __asm__("getsockopt")
# else
            __asm__("_getsockopt")
# endif
                ;
    }  // namespace posix
#endif

}  // namespace uwvm2::imported::wasi::wasip1::func

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_pop_macro.h>
#endif

