﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/// @brief      codecvt parameters on winnt
/// @details    In Windows NT, process parameters are stored in the Process Environment Block (PEB) as UTF-16 encoded strings.
///             When a new process is created, the CreateProcess function passes these UTF-16 encoded command-line arguments
///             to the new process. However, in environments where the system's Active Code Page (ACP) is set to a non-UTF-8 encoding,
///             such as GBK, the C runtime (crt) of the new process converts these UTF-16 arguments into the current ACP encoding when
///             initializing the char** argv array for the main function. This means that even if UTF-16 encoded arguments are passed
///             from the parent process, they are converted to the system's ACP (e.g., GBK) in the child process. To handle this,
///             applications can retrieve the original UTF-16 command-line arguments using the CommandLineToArgvW function and perform
///             necessary conversions to UTF-8 or other desired encodings as needed.​
///
///             In contrast, Windows 9x systems do not exhibit this behavior. Internally, command-line arguments are stored as char* strings,
///             and the system assumes a consistent encoding based on the system's code page. When using CreateProcess, the arguments passed
///             to the new process are received in the same encoding without any automatic conversion, maintaining the original byte representation.
///
///             This file introduces a global variable ::fast_io::u8string
///
/// @author     MacroModel
/// @version    2.0.0
/// @date       2025-03-25
/// @copyright  APL-2 License

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
import ulte.utils.ansies;
import ulte.uwvm.io;
import ulte.uwvm.utils.ansies;
#else
// std
# include <cstdint>
# include <cstddef>
# include <memory>
# include <bit>
// macro
# include <ulte/utils/macro/push_macros.h>
# include <ulte/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
# include <ulte/utils/ansies/impl.h>
# include <ulte/uwvm/io/impl.h>
# include <ulte/uwvm/utils/ansies/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace ulte::uwvm::cmdline
{
#if defined(_WIN32) && !defined(_WIN32_WINDOWS)
    struct nt_code_cvt_argv_storage UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        ::fast_io::vector<char8_t const*> argv{};
        ::fast_io::u8string parameter_sequence{};
        ::std::size_t argc{};
    };

    struct u16_cmdline_argv_guard UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        char16_t** argv{};

        inline constexpr u16_cmdline_argv_guard() noexcept = default;

        inline constexpr u16_cmdline_argv_guard(char16_t** other_argv) noexcept : argv{other_argv} {}

        u16_cmdline_argv_guard(u16_cmdline_argv_guard const&) = delete;
        u16_cmdline_argv_guard& operator= (u16_cmdline_argv_guard const&) = delete;

        inline constexpr u16_cmdline_argv_guard(u16_cmdline_argv_guard&& other) noexcept : argv{other.argv} { other.argv = nullptr; }

        inline constexpr u16_cmdline_argv_guard& operator= (u16_cmdline_argv_guard&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }
            this->argv = other.argv;
            other.argv = nullptr;
            return *this;
        }

        inline constexpr ~u16_cmdline_argv_guard() { clear(); }

        inline constexpr void clear() noexcept
        {
            if(this->argv) [[likely]] { ::fast_io::win32::LocalFree(this->argv); }
        }
    };

    /// @brief nt_code_cvt_argv and return nt_code_cvt_argv_storage
    inline nt_code_cvt_argv_storage nt_code_cvt_argv() noexcept
    {
        // get utf-16 cmdline from peb
        // There is no need to check the nullptr in this step, it's never nullptr.
        auto const nt_proc_para{::fast_io::win32::nt::nt_get_current_peb()->ProcessParameters};
        // There is no need to check the nullptr in this step, CommandLineToArgvW has special handling for nullptr.
        auto const nt_proc_cmdline{nt_proc_para->CommandLine};

        int u16_cmdline_argc;  // No initialization required
        auto const u16_cmdline_argv{::fast_io::win32::CommandLineToArgvW(nt_proc_cmdline.Buffer, ::std::addressof(u16_cmdline_argc))};
        if(u16_cmdline_argv == nullptr) [[unlikely]]
        {
            ::fast_io::io::perr(::ulte::uwvm::u8log_output,
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[fatal] ",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"CommandLineToArgvW failed: ",
                                ::fast_io::error{::fast_io::win32_domain_value, ::fast_io::win32::GetLastError()},
                                u8"\n",
                                ::fast_io::mnp::cond(::ulte::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"Terminate.\n\n");
            ::fast_io::fast_terminate();
        }

        u16_cmdline_argv_guard guard{u16_cmdline_argv};  // use raii to release

        nt_code_cvt_argv_storage res{};
        res.argc = static_cast<::std::size_t>(u16_cmdline_argc);

        res.argv.reserve(res.argc + 1);  // storage latest nullptr

        ::fast_io::u8ostring_ref_fast_io u8_storage_ref{::std::addressof(res.parameter_sequence)};

        for(auto curr_argv{u16_cmdline_argv}; curr_argv != u16_cmdline_argv + res.argc; ++curr_argv)
        {
            res.argv.push_back_unchecked(::std::bit_cast<char8_t const*>(res.parameter_sequence.size()));
            ::fast_io::operations::print_freestanding<false>(u8_storage_ref, ::fast_io::mnp::code_cvt_os_c_str(*curr_argv), u8"\0");
        }

        for(auto const res_ps_cbegin{res.parameter_sequence.cbegin()}; auto& curr_res_argv: res.argv)
        {
            curr_res_argv = res_ps_cbegin + ::std::bit_cast<::std::size_t>(curr_res_argv);
        }

        res.argv.push_back_unchecked(nullptr);

        return res;
    }

#endif
}  // namespace ulte::uwvm::cmdline

#ifndef UWVM_MODULE
// macro
# include <ulte/uwvm/utils/ansies/uwvm_color_pop_macro.h>
# include <ulte/utils/macro/pop_macros.h>
#endif
