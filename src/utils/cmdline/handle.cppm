/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-25
 * @copyright   APL-2 License
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

#include <cstdint>
#include <cstddef>
#include <memory>
#include <algorithm>

#include <utils/macro/push_macros.h>

export module utils.cmdline:handle;

import fast_io;

export namespace utils::cmdline
{
    /// @brief Used to indicate parameter type
    enum class parameter_parsing_results_type : unsigned
    {
        arg,                  // normal args
        occupied_arg,         // The occupied args are used for preprocessing or running wasm
        parameter,            // Correct parameters
        invalid_parameter,    // Invalid parameter
        duplicate_parameter,  // Duplicate parameter
        dir                   // The type of the first parameter, indicating the current program running path
    };

    struct parameter;

    /// @brief Parsing results of each crtmain parameter
    struct parameter_parsing_results
    {
        ::fast_io::u8cstring_view str{};        // Parameter content
        parameter const* para{};                // Corresponding parameter. If it is not a parameter, it is nullptr
        parameter_parsing_results_type type{};  // Parameter type
    };

    /// @brief Structure to storage parameter_parsing_results
    struct parameter_parsing_results_storage UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        using Alloc = ::fast_io::native_typed_global_allocator<parameter_parsing_results>;

        parameter_parsing_results* begin{};
        parameter_parsing_results* end{};

        inline constexpr parameter_parsing_results_storage() noexcept = default;

        inline constexpr parameter_parsing_results_storage(::std::size_t argc) noexcept
        {
            if UWVM_IF_CONSTEVAL { this->begin = ::new parameter_parsing_results[argc]; }
            else { this->begin = Alloc::allocate(argc); }
            this->end = this->begin + argc;
        }

        inline constexpr parameter_parsing_results_storage(parameter_parsing_results_storage const&) = delete;
        inline constexpr parameter_parsing_results_storage& operator= (parameter_parsing_results_storage const&) = delete;

        inline constexpr parameter_parsing_results_storage(parameter_parsing_results_storage&& other) noexcept : begin{other.begin}, end{other.end}
        {
            other.begin = nullptr;
            other.end = nullptr;
        }

        inline constexpr parameter_parsing_results_storage& operator= (parameter_parsing_results_storage&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            this->begin = other.begin;
            other.begin = nullptr;
            this->end = other.end;
            other.end = nullptr;
            return *this;
        }

        inline constexpr ~parameter_parsing_results_storage() { clear(); }

        inline constexpr void clear() noexcept
        {
            if(this->begin) [[likely]]
            {
                if UWVM_IF_CONSTEVAL { ::delete[] this->begin; }
                else
                {
                    auto const dealloc_size{static_cast<::std::size_t>(this->end - this->begin)};
                    Alloc::deallocate_n(this->begin, dealloc_size);
                }
            }
        }
    };

    /// @brief Used to indicate the return type of the parameter parser
    enum class parameter_return_type : unsigned
    {
        def,             // Continue with the program
        return_m1_imme,  // return -1 immediately
        return_imme,     // return 0 immediately
        return_soon,     // return 0 after parsing
        err_imme,        // trap immediately
        err_soon         // trap after parsing
    };

    /// @brief type for consecutive alias
    using kns_u8_str_scatter_t = ::fast_io::basic_io_scatter_t<::fast_io::u8string_view>;

    /// @brief type for handle function
    using handle_func_type = parameter_return_type (*)(parameter_parsing_results* para_begin,
                                                       parameter_parsing_results* para_curr,
                                                       parameter_parsing_results* para_end) noexcept;

    /// @brief User-defined parameters and handlers
    struct parameter
    {
        ::fast_io::u8string_view const name{};      // parameter name
        ::fast_io::u8string_view const describe{};  // describtion shown in help
        kns_u8_str_scatter_t alias{};               // alias names
        handle_func_type handle{};                  // formal processing results
        handle_func_type pretreatment{};            // pretreatment
        bool* is_exist{};                           // When it is not nullptr, repeated errors will be reported
    };

    /// @brief      sort the parameter const* const [N]
    /// @details    Sort user-defined parameters.
    ///             This function only allows consteval to implement.
    template <::std::size_t N>
    inline consteval auto parameter_sort(parameter const* const (&punsort)[N]) noexcept
    {
        ::fast_io::array<parameter const*, N> res{};
        for(::std::size_t i{}; i < N; ++i) { res.index_unchecked(i) = punsort[i]; }
        ::std::ranges::sort(res, [](parameter const* const a, parameter const* const b) noexcept -> bool { return a->name < b->name; });
        return res;
    }

    /// @brief This structure is used to store the parameters of all aliases
    struct alias_parameter
    {
        ::fast_io::u8string_view str{};  // raw name or alias name
        parameter const* para{};         // Equivalent parameter
    };

    /// @brief      Calculate all parameters sizes
    /// @details    Calculate the total number of aliases in the sorting parameter results obtained by parameter_sort
    ///             This function only allows consteval to implement.
    template <::std::size_t N>
    inline consteval ::std::size_t calculate_all_parameters_size(::fast_io::array<parameter const*, N> const& punsort) noexcept
    {
        ::std::size_t res{};
        for(::std::size_t i{}; i < N; ++i)
        {
            ++res;                                         // raw name size == 1
            res += punsort.index_unchecked(i)->alias.len;  // alias name size == alias.len
        }
        return res;
    }

}  // namespace utils::cmdline

/// @brief set fast_io::freestanding to match with fast_io
namespace fast_io::freestanding
{
    template <>
    struct is_trivially_copyable_or_relocatable<::utils::cmdline::parameter_parsing_results_storage>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::utils::cmdline::parameter_parsing_results_storage>
    {
        inline static constexpr bool value = true;
    };
}  // namespace fast_io::freestanding
