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
# include <limits>
# include <memory>
# include <new>
# include <atomic>
# include <bit>
# include <utility>
// macro
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_push_macro.h>
# include <uwvm2/utils/macro/push_macros.h>
// platform
# if defined(__APPLE__) || defined(__DARWIN_C_LEVEL)
#  include <crt_externs.h>
#  include <unistd.h>
# endif
# if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(BSD) || defined(_SYSTYPE_BSD) ||          \
     defined(__OpenBSD__)
#  include <sys/types.h>
#  include <sys/param.h>
#  include <sys/sysctl.h>
#  include <unistd.h>
# endif
// import
# include <fast_io.h>
# include <uwvm2/uwvm_predefine/io/impl.h>
# include <uwvm2/uwvm_predefine/utils/ansies/impl.h>
# include <uwvm2/utils/container/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::imported::wasi::wasip1::platform
{
    struct buf_guard_t
    {
        using allocator_t = ::fast_io::native_thread_local_allocator;

        void* buf{};

        inline constexpr buf_guard_t(void* buf_o) noexcept : buf{buf_o} {}

        inline constexpr buf_guard_t(buf_guard_t const& other) noexcept = delete;
        inline constexpr buf_guard_t& operator= (buf_guard_t const& other) noexcept = delete;
        inline constexpr buf_guard_t(buf_guard_t&& other) noexcept = delete;
        inline constexpr buf_guard_t& operator= (buf_guard_t&& other) noexcept = delete;

        inline constexpr ~buf_guard_t()
        {
            if(buf) [[likely]] { allocator_t::deallocate(buf); }
        }
    };

    inline ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string> get_process_env() noexcept
    {
        ::uwvm2::utils::container::vector<::uwvm2::utils::container::u8string> result{};

#if defined(_WIN32) || defined(__CYGWIN__)
# if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)
        // Windows 9x

        struct fast_io_get_environment_string_a_guard_t
        {
            char* tmp_environ{};

            inline constexpr fast_io_get_environment_string_a_guard_t(char* tmp_environ_o) noexcept : tmp_environ{tmp_environ_o} {}

            inline constexpr fast_io_get_environment_string_a_guard_t(fast_io_get_environment_string_a_guard_t const& other) noexcept = delete;
            inline constexpr fast_io_get_environment_string_a_guard_t& operator= (fast_io_get_environment_string_a_guard_t const& other) noexcept = delete;
            inline constexpr fast_io_get_environment_string_a_guard_t(fast_io_get_environment_string_a_guard_t&& other) noexcept = delete;
            inline constexpr fast_io_get_environment_string_a_guard_t& operator= (fast_io_get_environment_string_a_guard_t&& other) noexcept = delete;

            inline constexpr ~fast_io_get_environment_string_a_guard_t()
            {
                if(tmp_environ) [[likely]] { ::fast_io::win32::FreeEnvironmentStringsA(tmp_environ); }
                // multi-call to destructor is ub
            }
        };

        auto env_str{::fast_io::win32::GetEnvironmentStringsA()};
        fast_io_get_environment_string_a_guard_t env_str_guard{env_str};

        // Do not perform APC escaping from ANSI to UTF-8 under Windows 9x.
        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        for(auto env_str_curr{reinterpret_cast<char8_t_const_may_alias_ptr>(env_str)};;)
        {
            auto const pos{env_str_curr + ::fast_io::cstr_len(env_str_curr)};

            result.emplace_back(env_str_curr, pos);

            env_str_curr = pos + 1u;

            if(*env_str_curr == u8'\0') { break; }
        }

        return result;

# else
        auto const c_peb{::fast_io::win32::nt::nt_get_current_peb()};
        // c_peb->ProcessParameters cannot be null.
        auto const c_peb_environment{c_peb->ProcessParameters->Environment};
        auto const c_peb_environment_size{c_peb->ProcessParameters->EnvironmentSize};
        auto const c_peb_environment_end{c_peb_environment + c_peb_environment_size / sizeof(char16_t)};

        // Converting everything at once is faster than converting one by one.
        auto const env_str{
            ::uwvm2::utils::container::tlc::u8concat_uwvm_tlc(::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(c_peb_environment, c_peb_environment_end)))};

        auto const env_str_end{env_str.cend()};

        for(auto env_str_curr{env_str.cbegin()};;)
        {
            auto const len{::fast_io::cstr_nlen(env_str_curr, static_cast<::std::size_t>(env_str_end - env_str_curr))};
            if(len == 0uz) [[unlikely]] { break; }
            auto const next{env_str_curr + len};

            result.emplace_back(env_str_curr, next);

            if(next >= env_str_end) { break; }
            env_str_curr = next + 1u;
        }

        return result;

# endif
#elif defined(__MSDOS__) || defined(__DJGPP__)
        extern char** _environ;
        auto const environ_begin{_environ};

        if(environ_begin == nullptr) [[unlikely]] { return result; }

        for(auto environ_curr{environ_begin}; *environ_curr != nullptr; ++environ_curr)
        {
            result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt_os_c_str(*environ_curr)));
        }

        return result;

#elif defined(__APPLE__) || defined(__DARWIN_C_LEVEL)
        auto const envpp{::fast_io::noexcept_call(::_NSGetEnviron)};
        if(envpp == nullptr) [[unlikely]] { return result; }

        auto const environ_begin{*envpp};
        if(environ_begin == nullptr) [[unlikely]] { return result; }

        for(auto environ_curr{environ_begin}; *environ_curr != nullptr; ++environ_curr)
        {
            result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt_os_c_str(*environ_curr)));
        }

        return result;

#elif defined(__linux__) || defined(__sun)

        ::fast_io::u8native_file envs;  // no initialize
        ::std::size_t envs_file_size;   // no initialize
# ifdef UWVM_CPP_EXCEPTIONS
        try
# endif
        {
            envs = ::fast_io::u8native_file{u8"/proc/self/environ", ::fast_io::open_mode::in};
            envs_file_size = file_size(envs);
        }
# ifdef UWVM_CPP_EXCEPTIONS
        catch(::fast_io::error)
        {
            return result;
        }
# endif

        auto const buf{reinterpret_cast<char8_t*>(buf_guard_t::allocator_t::allocate(envs_file_size * sizeof(char8_t)))};
        buf_guard_t buf_guard{buf};

        auto const env_buf_end{::fast_io::operations::read_some(envs, buf, buf + envs_file_size)};

        for(auto env_str_curr{buf};;)
        {
            auto const len{::fast_io::cstr_nlen(env_str_curr, static_cast<::std::size_t>(env_buf_end - env_str_curr))};
            if(len == 0uz) [[unlikely]] { break; }

            auto const next{env_str_curr + len};
            result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(env_str_curr, next))));

            if(next >= env_buf_end) { break; }
            env_str_curr = next + 1u;
        }

        return result;

#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(BSD) || defined(_SYSTYPE_BSD) ||         \
    defined(__OpenBSD__)
        // BSD family: robustly collect environment via sysctl where applicable; fallback to environ

        bool filled_via_sysctl{};

# if defined(KERN_PROC_ENV) || defined(__NetBSD__)
        // FreeBSD/NetBSD/DragonFly: KERN_PROC... interface
        // FreeBSD warning: sysctl may truncate without ENOMEM; mitigate by two-call sizing and growth loop
        int mib[4];  // no initialize

#  if defined(__NetBSD__)
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC_ARGS;
        mib[2] = ::fast_io::noexcept_call(::getpid);
        mib[3] = KERN_PROC_ENV;
#  else
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_ENV;
        mib[3] = ::fast_io::noexcept_call(::getpid);
#  endif

        ::std::size_t size{};
        // Probe size; if the kernel returns 0 or small, still attempt read with exponential buffer growth

        if(::fast_io::noexcept_call(::sysctl, mib, 4, nullptr, ::std::addressof(size), nullptr, 0) == 0 && size != 0u) [[likely]]
        {
            auto const buf{reinterpret_cast<char*>(buf_guard_t::allocator_t::allocate(size))};
            buf_guard_t buf_guard{buf};

            if(::fast_io::noexcept_call(::sysctl, mib, 4, buf, ::std::addressof(size), nullptr, 0) == 0)
            {
                auto const env_str_end{buf + size};

                for(auto env_str_curr{buf};;)
                {
                    auto const len{::fast_io::cstr_nlen(env_str_curr, static_cast<::std::size_t>(env_str_end - env_str_curr))};
                    if(len == 0uz) [[unlikely]] { break; }

                    auto const next{env_str_curr + len};
                    result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(env_str_curr, next))));

                    if(next >= env_str_end) { break; }
                    env_str_curr = next + 1u;
                }

                filled_via_sysctl = true;
            }
        }
# endif

        if(!filled_via_sysctl) [[unlikely]]
        {
            // Fallback: use process environ when sysctl is unavailable or permission denied
            extern char** environ;
            auto const environ_begin{environ};

            if(environ_begin == nullptr) [[unlikely]] { return result; }

            for(auto environ_curr{environ_begin}; *environ_curr != nullptr; ++environ_curr)
            {
                result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt_os_c_str(*environ_curr)));
            }
        }

        return result;

#else
        // Unknown platform: fallback to POSIX environ if available
        extern char** environ;
        auto const environ_begin{environ};

        if(environ_begin == nullptr) [[unlikely]] { return result; }

        for(auto environ_curr{environ_begin}; *environ_curr != nullptr; ++environ_curr)
        {
            result.push_back(::uwvm2::utils::container::u8concat_uwvm(::fast_io::mnp::code_cvt_os_c_str(*environ_curr)));
        }

        return result;
#endif
    }

}  // namespace uwvm2::imported::wasi::wasip1::platform

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
# include <uwvm2/uwvm_predefine/utils/ansies/uwvm_color_pop_macro.h>
#endif
