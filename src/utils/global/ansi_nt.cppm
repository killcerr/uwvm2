/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author MacroModel
 * @version 2.0.0
 * @details unfinished
 * @date 2025-03-21
 * @copyright APL-2 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

module;

/// @brief include std
#include <cstdint>
#include <memory>

/// @brief include source_location for debug mode
#if defined(_DEBUG)
#include <source_location>
#endif

/// @brief utils.global:ansi_nt module declaration
export module utils.global:ansi_nt;

/// @brief import fast_io module
import fast_io;

/// @brief import utils.io:io_device module
import utils.io;

export namespace uwvm::global
{
    /// @brief      cmd on windows nt does not enable ansi escaping by default.
    /// @details    Declare this via a global variable to set up ansi escaping at program runtime.
    ///             Because of the CsrClientCallServer behavior, it must be restored at the end of the program.
    struct enable_nt_ansi
    {
        inline static constexpr ::std::uint_least32_t enable_virtual_terminal_processing{0x0004u};

        ::std::uint_least32_t out_omode{};
        ::std::uint_least32_t err_omode{};

        void* out_handle{};
        void* err_handle{};

        // Don't just use any other global variable in global, it may be uninitialized
        
#if __has_cpp_attribute(__gnu__::__cold__)
        [[__gnu__::__cold__]]
#endif
        inline enable_nt_ansi() noexcept
        {
#if defined(_DEBUG)
            ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] try enable_nt_ansi(): ", ::std::source_location::current());
#endif
            auto proc_parameter{::fast_io::win32::nt::nt_get_current_peb()->ProcessParameters};
            out_handle = proc_parameter->StandardOutput;
            err_handle = proc_parameter->StandardError;

            ::fast_io::win32::nt::console_api_message apimsg;  // No initialization required
            auto ConsoleModeRequest{::std::addressof(apimsg.Data.ConsoleModeRequest)};
            ConsoleModeRequest->ConsoleHandle = proc_parameter->ConsoleHandle;

            // get and set out
            ConsoleModeRequest->Handle = out_handle;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepGetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }

            out_omode = ConsoleModeRequest->Mode;

            ConsoleModeRequest->Mode |= enable_virtual_terminal_processing;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepSetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }

            // get and set err
            ConsoleModeRequest->Handle = err_handle;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepGetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }

            err_omode = ConsoleModeRequest->Mode;

            ConsoleModeRequest->Mode |= enable_virtual_terminal_processing;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepSetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }

#if defined(_DEBUG)
            ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() success: ", ::std::source_location::current());
#endif

        }

#if __has_cpp_attribute(__gnu__::__cold__)
        [[__gnu__::__cold__]]
#endif
        inline ~enable_nt_ansi() noexcept
        {
#if defined(_DEBUG)
            ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] try ~enable_nt_ansi(): ", ::std::source_location::current());
#endif

            auto proc_parameter{::fast_io::win32::nt::nt_get_current_peb()->ProcessParameters};

            ::fast_io::win32::nt::console_api_message apimsg;  // No initialization required
            auto ConsoleModeRequest{::std::addressof(apimsg.Data.ConsoleModeRequest)};
            ConsoleModeRequest->ConsoleHandle = proc_parameter->ConsoleHandle;

            // set out
            ConsoleModeRequest->Handle = out_handle;
            ConsoleModeRequest->Mode = out_omode;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepSetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }

            // set err
            ConsoleModeRequest->Handle = err_handle;
            ConsoleModeRequest->Mode = err_omode;

            ::fast_io::win32::nt::CsrClientCallServer(::std::addressof(apimsg),
                                                      nullptr,
                                                      static_cast<::std::uint_least32_t>(2) << 16 |
                                                          static_cast<::std::uint_least32_t>(::fast_io::win32::nt::consrv_api_number::ConsolepSetMode),
                                                      sizeof(apimsg.Data.ConsoleModeRequest));

            if(apimsg.Status) [[unlikely]]
            {
#if defined(_DEBUG)
                ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] enable_nt_ansi() fatal: ", ::std::source_location::current());
#endif
                return;  // do nothing, just return
            }
#if defined(_DEBUG)
            ::fast_io::io::perrln(::uwvm::u8err, u8"[debug] ~enable_nt_ansi() success: ", ::std::source_location::current());
#endif
        }
    };
}  // namespace uwvm::global
