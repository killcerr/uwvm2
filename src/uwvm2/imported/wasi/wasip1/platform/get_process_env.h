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
        auto const c_peb_environment{c_peb->ProcessParameters->Environment};
        auto const c_peb_environment_size{c_peb->ProcessParameters->EnvironmentSize};
        auto const c_peb_environment_end{c_peb_environment + c_peb_environment_size};

        ::fast_io::tlc::u8string env_str{
            ::fast_io::tlc::u8concat_fast_io_tlc(::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(c_peb_environment, c_peb_environment_end)))};

        auto const env_str_end{env_str.cend()};

        for(auto env_str_curr{env_str.cbegin()};;)
        {
            auto const pos{::std::find(env_str_curr, env_str_end, u8'\0')};

            result.emplace_back(env_str_curr, pos);

            env_str_curr = pos + 1u;

            if(*env_str_curr == u8'\0') { break; }
        }

        return result;
# endif
#elif defined(__APPLE__) || defined(__DARWIN_C_LEVEL)
        // macOS: use _NSGetEnviron to obtain process environ pointer
        using ::uwvm2::utils::container::u8string;
        using ::uwvm2::utils::container::vector;
        char*** envpp = ::fast_io::noexcept_call(::_NSGetEnviron);
        if(envpp == nullptr) { return result; }
        char** envp = *envpp;
        if(envp == nullptr) { return result; }
        for(char** p = envp; *p != nullptr; ++p)
        {
            auto s = *p;
            auto len = ::fast_io::cstr_len(s);
            ::uwvm2::utils::container::u8string str;
            str.resize(len);
            ::fast_io::freestanding::my_memcpy(str.data(), s, len);
            result.push_back(::std::move(str));
        }
        return result;

#elif defined(__linux__) || defined(__sun)
        // Linux/Solaris: use POSIX environ (robust and sufficient)
        extern "C" char** environ;
        if(environ == nullptr) { return result; }
        for(char** p = environ; *p != nullptr; ++p)
        {
            auto s = *p;
            auto len = ::fast_io::cstr_len(s);
            ::uwvm2::utils::container::u8string u8;
            u8.resize(len);
            ::fast_io::freestanding::my_memcpy(u8.data(), s, len);
            result.push_back(::std::move(u8));
        }
        return result;

#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(BSD) || defined(_SYSTYPE_BSD) ||         \
    defined(__OpenBSD__)
        // BSD family: robustly collect environment via sysctl where applicable; fallback to environ
        using ::uwvm2::utils::container::u8string;
        using ::uwvm2::utils::container::vector;
        bool filled_via_sysctl = false;

# if defined(__OpenBSD__)
        // OpenBSD requires a two-step sysctl to get size safely
        int mib[4]{CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_ENV};
        ::std::size_t size{};
        if(::fast_io::noexcept_call(::sysctl, mib, 4, nullptr, &size, nullptr, 0) == 0 && size != 0u)
        {
            ::uwvm2::utils::container::u8string buf;
            buf.resize(size);
            if(::fast_io::noexcept_call(::sysctl, mib, 4, buf.data(), &size, nullptr, 0) == 0)
            {
                // Trim to actual size
                if(size < buf.size()) { buf.resize(size); }
                auto* beg = buf.data();
                auto* end = buf.data() + buf.size();
                while(beg < end)
                {
                    char8_t* pos = beg;
                    while(pos < end && *pos != u8'\0') { ++pos; }
                    if(pos == beg && pos == end) { break; }
                    result.push_back(u8string{beg, static_cast<::std::size_t>(pos - beg)});
                    if(pos == end) { break; }
                    beg = pos + 1;
                }
                filled_via_sysctl = true;
            }
        }
# else
        // FreeBSD/NetBSD/DragonFly: KERN_PROC... interface
        // FreeBSD warning: sysctl may truncate without ENOMEM; mitigate by two-call sizing and growth loop
        int mib[4];
#  if defined(__NetBSD__)
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC_ARGS;
        mib[2] = getpid();
        mib[3] = KERN_PROC_ENV;
#  else
        mib[0] = CTL_KERN;
        mib[1] = KERN_PROC;
        mib[2] = KERN_PROC_ENV;
        mib[3] = getpid();
#  endif
        ::std::size_t size{0};
        // Probe size; if the kernel returns 0 or small, still attempt read with exponential buffer growth
        (void)::fast_io::noexcept_call(::sysctl, mib, 4, nullptr, &size, nullptr, 0);
        if(size == 0u) { size = 4096u; }
        for(int attempt = 0; attempt < 6; ++attempt)
        {
            ::uwvm2::utils::container::u8string buf;
            buf.resize(size);
            ::std::size_t cursize = size;
            int rc = ::fast_io::noexcept_call(::sysctl, mib, 4, buf.data(), &cursize, nullptr, 0);
            if(rc == 0)
            {
                if(cursize < buf.size()) { buf.resize(cursize); }
                auto* beg = buf.data();
                auto* end = buf.data() + buf.size();
                while(beg < end)
                {
                    char8_t* pos = beg;
                    while(pos < end && *pos != u8'\0') { ++pos; }
                    if(pos == beg && pos == end) { break; }
                    result.push_back(u8string{beg, static_cast<::std::size_t>(pos - beg)});
                    if(pos == end) { break; }
                    beg = pos + 1;
                }
                filled_via_sysctl = true;
                break;
            }
            // If failed, grow buffer and retry to mitigate silent truncation
            size *= 2u;
        }
# endif
        if(!filled_via_sysctl)
        {
            // Fallback: use process environ when sysctl is unavailable or permission denied
            extern "C" char** environ;
            if(environ)
            {
                for(char** p = environ; *p != nullptr; ++p)
                {
                    auto s = *p;
                    auto len = ::fast_io::cstr_len(s);
                    ::uwvm2::utils::container::u8string u8;
                    u8.resize(len);
                    ::fast_io::freestanding::my_memcpy(u8.data(), s, len);
                    result.push_back(::std::move(u8));
                }
            }
        }
        return result;

#else
        // Unknown platform: fallback to POSIX environ if available
        extern "C" char** environ;
        if(environ)
        {
            for(char** p = environ; *p != nullptr; ++p)
            {
                auto s = *p;
                auto len = ::fast_io::cstr_len(s);
                ::uwvm2::utils::container::u8string u8;
                u8.resize(len);
                ::fast_io::freestanding::my_memcpy(u8.data(), s, len);
                result.push_back(::std::move(u8));
            }
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
