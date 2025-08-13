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

#ifndef UWVM_MODULE
// std
# include <cstddef>
# include <cstdint>
# include <cstring>
# include <climits>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <limits>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <uwvm2/utils/container/impl.h>
# include <uwvm2/utils/debug/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/utils/impl.h>
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
    struct global_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::uwvm2::utils::container::u8string_view section_name{u8"Global"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::global_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::uwvm2::utils::container::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_local_global_type<Fs...>> local_globals{};
    };

    /// @brief define handler for ::uwvm2::parser::wasm::standard::wasm1::type::global_type
    /// @note  ADL for distribution to the correct handler function
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* global_section_global_handler(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<global_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::standard::wasm1::type::global_type & global_r,  // [adl] can be replaced
        [[maybe_unused]] ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) UWVM_THROWS
    {
        // [... global_curr] ...
        // [   safe        ] unsafe (could be the section_end)
        //      ^^ section_curr

        // Handling of scan_global_type is completely memory-safe

        return ::uwvm2::parser::wasm::standard::wasm1::features::scan_global_type(global_r, section_curr, section_end, err);
    }

    /// @brief Define the handler function for global_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<global_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::std::byte const* const sec_id_module_ptr) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse global section (id: 6)"};
#endif
        // Note that section_begin may be equal to section_end
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // get global_section_storage_t from storages
        auto& globalsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<global_section_storage_t<Fs...>>(module_storage.sections)};

        // import section
        auto const& importsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(module_storage.sections)};
        // importdesc[3]: global
        constexpr ::std::size_t importdesc_count{importsec.importdesc_count};
        static_assert(importdesc_count > 3uz);  // Ensure that subsequent index visits do not cross boundaries
        auto const imported_global_size{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(importsec.importdesc.index_unchecked(3uz).size())};

        // check duplicate
        if(globalsec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8 = globalsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        globalsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        globalsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        // [before_section ... ] | global_count ... global1 ...
        // [        safe       ] | unsafe (could be the section_end)
        //                         ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 global_count;  // No initialization necessary

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        auto const [global_count_next, global_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                  reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                  ::fast_io::mnp::leb128_get(global_count))};

        if(global_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_global_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(global_count_err);
        }

        // [before_section ... | global_count ...] global1 ...
        // [             safe                    ] unsafe (could be the section_end)
        //                       ^^ section_curr

        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(global_count > size_t_max) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u64 = static_cast<::std::uint_least64_t>(global_count);
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        if(global_count > wasm_u32_max - imported_global_size) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.imp_def_num_exceed_u32max.type = 0x03;  // global
            err.err_selectable.imp_def_num_exceed_u32max.defined = global_count;
            err.err_selectable.imp_def_num_exceed_u32max.imported = imported_global_size;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::imp_def_num_exceed_u32max;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        globalsec.local_globals.reserve(static_cast<::std::size_t>(global_count));

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 global_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(global_count_next);  // never out of bounds
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // [before_section ... | global_count ...] global1 ...
        // [              safe                   ] unsafe (could be the section_end)
        //                                         ^^ section_curr

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [... global_curr] ...
            // [   safe        ] unsafe (could be the section_end)
            //      ^^ section_curr

            // check global counter
            // Ensure content is available before counting (section_curr != section_end)
            if(::uwvm2::parser::wasm::utils::counter_selfinc_throw_when_overflow(global_counter, section_curr, err) > global_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = global_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // storage global (need move)
            ::uwvm2::parser::wasm::standard::wasm1::features::final_local_global_type<Fs...> local_global{};

            // [... global_curr] ... expr_curr ... 0x0B global_next ...
            // [   safe        ] unsafe (could be the section_end)
            //      ^^ section_curr

            // Function call matching via adl
            static_assert(::uwvm2::parser::wasm::standard::wasm1::features::has_global_section_global_handler<Fs...>);

            section_curr = global_section_global_handler(sec_adl, local_global.global, module_storage, section_curr, section_end, err, fs_para);

            // [... global_curr ...] expr_curr ... 0x0B global_next ...
            // [   safe            ] unsafe (could be the section_end)
            //                       ^^ section_curr

            // The initialization of global is not done here, its initialization constant expression contains the imported global, which can only be initialized
            // after all modules have been imported, so it will be initialized before the execution of the wasm module.

            local_global.expr.begin = section_curr;

            // [... global_curr ...] expr_curr ... 0x0B global_next ...
            // [   safe            ] unsafe (could be the section_end)
            //                       ^^ local_global.expr.begin

            // The calculation of constant expressions will be parsed during the initialization phase.

            for(;; ++section_curr)
            {
                if(section_curr == section_end) [[unlikely]]
                {
                    // [... global_curr ... expr_curr ...] (end) ...
                    // [                   safe          ] unsafe (could be the section_end)
                    //                                     ^^ section_curr

                    err.err_curr = section_curr;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_init_terminator_not_found;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                // [... curr] ...
                // [  safe  ] unsafe (could be the section_end)
                //      ^^ section_curr

                ::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type section_curr_c8;
                ::std::memcpy(::std::addressof(section_curr_c8), section_curr, sizeof(::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type));

                // Size equal to one does not need to do little-endian conversion
                static_assert(sizeof(section_curr_c8) == 1uz);

#if CHAR_BIT > 8
                section_curr_c8 &= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0xFFu);
#endif

                if(section_curr_c8 == static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0x0Bu)) { break; }
            }

            // [... global_curr ... expr_curr ... 0x0B] global_next ...
            // [                   safe               ] unsafe (could be the section_end)
            //                                    ^^ section_curr

            ++section_curr;

            // [... global_curr ... expr_curr ... 0x0B] global_next ...
            // [                   safe               ] unsafe (could be the section_end)
            //                                          ^^ section_curr
            //                                          ^^ local_global.expr.end

            local_global.expr.end = section_curr;

            globalsec.local_globals.push_back_unchecked(::std::move(local_global));
        }

        // [... ] (section_end)
        // [safe] unsafe (could be the section_end)
        //        ^^ section_curr

        // check global counter match
        if(global_counter != global_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = global_counter;
            err.err_selectable.u32arr[1] = global_count;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::global_section_resolved_not_match_the_actual_number;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }

    /// @brief Wrapper for the global section storage structure
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct global_section_storage_section_details_wrapper_t
    {
        global_section_storage_t<Fs...> const* global_section_storage_ptr{};
        ::uwvm2::parser::wasm::binfmt::ver1::splice_section_storage_structure_t<Fs...> const* all_sections_ptr{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr global_section_storage_section_details_wrapper_t<Fs...> section_details(
        global_section_storage_t<Fs...> const& global_section_storage,
        ::uwvm2::parser::wasm::binfmt::ver1::splice_section_storage_structure_t<Fs...> const& all_sections) noexcept
    {
        return {::std::addressof(global_section_storage), ::std::addressof(all_sections)};
    }

    /// @brief Print the global section details
    /// @throws maybe throw fast_io::error, see the implementation of the stream
    template <::std::integral char_type, typename Stm, ::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void print_define(::fast_io::io_reserve_type_t<char_type, global_section_storage_section_details_wrapper_t<Fs...>>,
                                       Stm && stream,
                                       global_section_storage_section_details_wrapper_t<Fs...> const global_section_details_wrapper)
    {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
        if(global_section_details_wrapper.global_section_storage_ptr == nullptr || global_section_details_wrapper.all_sections_ptr == nullptr) [[unlikely]]
        {
            ::uwvm2::utils::debug::trap_and_inform_bug_pos();
        }
#endif

        auto const global_section_size{global_section_details_wrapper.global_section_storage_ptr->local_globals.size()};

        if(global_section_size)
        {
            if constexpr(::std::same_as<char_type, char>)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), "\nGlobal[", global_section_size, "]:\n");
            }
            else if constexpr(::std::same_as<char_type, wchar_t>)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), L"\nGlobal[", global_section_size, L"]:\n");
            }
            else if constexpr(::std::same_as<char_type, char8_t>)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), u8"\nGlobal[", global_section_size, u8"]:\n");
            }
            else if constexpr(::std::same_as<char_type, char16_t>)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), u"\nGlobal[", global_section_size, u"]:\n");
            }
            else if constexpr(::std::same_as<char_type, char32_t>)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), U"\nGlobal[", global_section_size, U"]:\n");
            }

            auto const& importsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<import_section_storage_t<Fs...>>(
                *global_section_details_wrapper.all_sections_ptr)};
            static_assert(importsec.importdesc_count > 3uz);
            auto global_counter{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(importsec.importdesc.index_unchecked(3uz).size())};

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 localdef_counter{};

            for(auto const& curr_global: global_section_details_wrapper.global_section_storage_ptr->local_globals)
            {
                if constexpr(::std::same_as<char_type, char>)
                {
                    ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream),
                                                                     " - localglobal[",
                                                                     localdef_counter,
                                                                     "] -> global[",
                                                                     global_counter,
                                                                     "]: {",
                                                                     section_details(curr_global.global),
                                                                     "}\n");
                }
                else if constexpr(::std::same_as<char_type, wchar_t>)
                {
                    ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream),
                                                                     L" - localglobal[",
                                                                     localdef_counter,
                                                                     L"] -> global[",
                                                                     global_counter,
                                                                     L"]: {",
                                                                     section_details(curr_global.global),
                                                                     L"}\n");
                }
                else if constexpr(::std::same_as<char_type, char8_t>)
                {
                    ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream),
                                                                     u8" - localglobal[",
                                                                     localdef_counter,
                                                                     u8"] -> global[",
                                                                     global_counter,
                                                                     u8"]: {",
                                                                     section_details(curr_global.global),
                                                                     u8"}\n");
                }
                else if constexpr(::std::same_as<char_type, char16_t>)
                {
                    ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream),
                                                                     u" - localglobal[",
                                                                     localdef_counter,
                                                                     u"] -> global[",
                                                                     global_counter,
                                                                     u"]: {",
                                                                     section_details(curr_global.global),
                                                                     u"}\n");
                }
                else if constexpr(::std::same_as<char_type, char32_t>)
                {
                    ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream),
                                                                     U" - localglobal[",
                                                                     localdef_counter,
                                                                     U"] -> global[",
                                                                     global_counter,
                                                                     U"]: {",
                                                                     section_details(curr_global.global),
                                                                     U"}\n");
                }

                ++global_counter;
                ++localdef_counter;
            }
        }
    }
}

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::global_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::global_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
