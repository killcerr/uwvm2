/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-07-03
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

#ifdef UWVM_MODULE
import fast_io;
# ifdef UWVM_TIMER
import uwvm2.utils.debug;
# endif
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.standard.wasm1.section;
import uwvm2.parser.wasm.standard.wasm1.opcode;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import :def;
import :feature_def;
import :types;
#else
// std
# include <cstddef>
# include <cstdint>
# include <cstring>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# ifdef UWVM_TIMER
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
# include "feature_def.h"
# include "types.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct memory_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Memory"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::memory_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...>> memories{};
    };

    /// @brief define handler for ::uwvm2::parser::wasm::standard::wasm1::type::memory_type
    /// @note  ADL for distribution to the correct handler function
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* memory_section_memory_handler(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<memory_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::standard::wasm1::type::memory_type & memory_r,  // [adl] can be replaced
        [[maybe_unused]] ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) UWVM_THROWS
    {
        // [... memory_curr] ...
        // [   safe        ] unsafe (could be the section_end)
        //      ^^ section_curr

        // Handling of scan_memory_type is completely memory-safe

        return ::uwvm2::parser::wasm::standard::wasm1::features::scan_memory_type(memory_r, section_curr, section_end, err);
    }

    /// @brief Define the handler function for memory_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<memory_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::std::byte const* const sec_id_module_ptr) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse memory section (id: 5)"};
#endif
        // Note that section_begin may be equal to section_end
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // get memory_section_storage_t from storages
        auto& memorysec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<memory_section_storage_t<Fs...>>(module_storage.sections)};

        // import section
        auto const& importsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(module_storage.sections)};
        // importdesc[2]: memory
        constexpr ::std::size_t importdesc_count{importsec.importdesc_count};
        static_assert(importdesc_count > 2uz);  // Ensure that subsequent index visits do not cross boundaries
        auto const imported_memory_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(importsec.importdesc.index_unchecked(2uz).size())};

        // check duplicate
        if(memorysec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8 = memorysec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        memorysec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        memorysec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        // [before_section ... ] | memory_count ... memory1 ...
        // [        safe       ] | unsafe (could be the section_end)
        //                         ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 memory_count;  // No initialization necessary

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [memory_count_next, memory_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                  reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                  ::fast_io::mnp::leb128_get(memory_count))};

        if(memory_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_memory_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(memory_count_err);
        }

        // [before_section ... | memory_count ...] memory1 ...
        // [             safe                    ] unsafe (could be the section_end)
        //                       ^^ section_curr

        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(memory_count > size_t_max) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u64 = static_cast<::std::uint_least64_t>(memory_count);
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        constexpr bool allow_multi_memory{::uwvm2::parser::wasm::standard::wasm1::features::allow_multi_memory<Fs...>()};
        if constexpr(!allow_multi_memory)
        {
            /// @details    In the current version of WebAssembly, at most one memory may be defined or imported in a single module,
            ///             and all constructs implicitly reference this memory 0. This restriction may be lifted in future versions
            /// @see        WebAssembly Release 1.0 (2019-07-20) § 2.5.5

            [[assume(imported_memory_size <= 1u)]];

            // memory_count is not incremental and requires overflow checking
            constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};

            if(memory_count > wasm_u32_max - imported_memory_size ||
               memory_count + imported_memory_size > static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u)) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::wasm1_not_allow_multi_memory;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }
        else
        {
            constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};

            if(memory_count > wasm_u32_max - imported_memory_size) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.imp_def_num_exceed_u32max.type = 0x02;  // memory
                err.err_selectable.imp_def_num_exceed_u32max.defined = memory_count;
                err.err_selectable.imp_def_num_exceed_u32max.imported = imported_memory_size;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::imp_def_num_exceed_u32max;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        memorysec.memories.reserve(static_cast<::std::size_t>(memory_count));

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 memory_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(memory_count_next);  // never out of bounds
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // [before_section ... | memory_count ...] memory1 ...
        // [              safe                   ] unsafe (could be the section_end)
        //                                         ^^ section_curr

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [... memory_curr] ...
            // [   safe        ] unsafe (could be the section_end)
            //      ^^ section_curr

            // check memory counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++memory_counter > memory_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = memory_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::memory_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // storage memory (need move)
            ::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...> memory{};

            // [... memory_curr] ...
            // [   safe        ] unsafe (could be the section_end)
            //      ^^ section_curr

            // Function call matching via adl
            static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_memory_section_memory_handler<Fs...>);

            section_curr = memory_section_memory_handler(sec_adl, memory, module_storage, section_curr, section_end, err, fs_para);

            // [... memory_curr ...] memory_next
            // [   safe            ] unsafe (could be the section_end)
            //                       ^^ section_curr

            memorysec.memories.push_back_unchecked(::std::move(memory));
        }

        // [... ] (section_end)
        // [safe] unsafe (could be the section_end)
        //        ^^ section_curr

        // check memory counter match
        if(memory_counter != memory_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = memory_counter;
            err.err_selectable.u32arr[1] = memory_count;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::memory_section_resolved_not_match_the_actual_number;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }
}  // namespace uwvm2::parser::wasm::standard::wasm1::features

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::memory_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::memory_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
