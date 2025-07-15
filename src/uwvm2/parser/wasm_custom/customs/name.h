/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-07-09
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
import uwvm2.utils.utf;
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard;
#else
// std
# include <cstddef>
# include <cstdint>
# include <cstring>
# include <concepts>
# include <type_traits>
# include <utility>
# include <map>  /// @todo use fast_io::string_hashmap instead
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string.h>
# include <fast_io_dsal/string_view.h>
# ifdef UWVM_TIMER
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/utils/utf/impl.h>
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/impl.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm_custom::customs
{
    struct name_storage_t
    {
        ::fast_io::u8string_view module_name{};
        ::std::map<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32, ::fast_io::u8string_view> function_name{};
        ::std::map<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32,
                   ::std::map<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32, ::fast_io::u8string_view>>
            code_local_name{};
    };

    enum class name_err_type_t : unsigned
    {
        invalid_name_map_length,
        size_exceeds_the_maximum_value_of_size_t,
        illegal_name_map_length,
        invalid_module_name_length,
        illegal_module_name_length,
        illegal_utf8_sequence,
        duplicate_module_name,
        duplicate_function_name,
        invalid_data_exists,
        invalid_function_name_count,
        invalid_function_index,
        invalid_function_name_length,
        illegal_section_id,
        illegal_function_name_length,
        duplicate_func_idx,
        duplicate_local_name,
        invalid_local_count,
        invalid_function_local_count,
        invalid_function_local_index,
        invalid_function_local_name_length,
        illegal_function_local_name_length,
        duplicate_code_function_index,
        duplicate_code_local_name_function_index
    };

    union name_err_storage_t
    {
        ::std::byte const* err_end;
        ::std::size_t err_uz;
        ::std::ptrdiff_t err_pdt;

        ::std::uint_least64_t u64;
        ::std::int_least64_t i64;
        ::std::uint_least32_t u32;
        ::std::int_least32_t i32;
        ::std::uint_least16_t u16;
        ::std::int_least16_t i16;
        ::std::uint_least8_t u8;
        ::std::int_least8_t i8;

        ::uwvm2::parser::wasm::base::error_f64 f64;
        ::uwvm2::parser::wasm::base::error_f32 f32;
        bool boolean;

        ::std::uint_least64_t u64arr[1];
        ::std::int_least64_t i64arr[1];
        ::std::uint_least32_t u32arr[2];
        ::std::int_least32_t i32arr[2];
        ::std::uint_least16_t u16arr[4];
        ::std::int_least16_t i16arr[4];
        ::std::uint_least8_t u8arr[8];
        ::std::int_least8_t i8arr[8];

        ::uwvm2::parser::wasm::base::error_f64 f64arr[1];
        ::uwvm2::parser::wasm::base::error_f32 f32arr[2];
        bool booleanarr[8];
    };

    struct name_err_t
    {
        ::std::byte const* curr{};
        name_err_type_t type{};
        name_err_storage_t err{};
    };

    inline constexpr void parse_name_storage(name_storage_t & ns,
                                             ::std::byte const* const begin,
                                             ::std::byte const* const end,
                                             ::fast_io::vector<name_err_t>& err) noexcept
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse custom section: name"};
#endif

        // Since wasm specifies that custom section processing failures should not throw exceptions
        // The custom section is not as strict as the standard section, so there is no need to extend the template type with variable parameters.

        auto curr{begin};

        // [... ] section_id ...
        // [safe] unsafe (could be the end)
        //        ^^ curr

        while(curr != end)
        {
            // [...  section_id] name_map_length ...
            // [     safe      ] unsafe (could be the end)
            //       ^^ curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id;

            ::std::memcpy(::std::addressof(section_id), curr, sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte));

            static_assert(sizeof(section_id) == 1uz);

            // Size equal to one does not need to do little-endian conversion

            // Avoid high invalid byte problem for platforms with CHAR_BIT greater than 8
#if CHAR_BIT > 8
            section_id &= 0xFFu;
#endif

            auto const section_id_ptr{curr};

            ++curr;

            // [...  section_id] name_map_length ...
            // [     safe      ] unsafe (could be the end)
            //                   ^^ curr

            // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)
            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 name_map_length;  // No initialization necessary

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [name_map_length_next, name_map_length_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(end),
                                                                                            ::fast_io::mnp::leb128_get(name_map_length))};

            if(name_map_length_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.emplace_back(curr, name_err_type_t::invalid_name_map_length);
                return;
            }

            // [...  section_id name_map_length ...] ...
            // [             safe                  ] unsafe (could be the end)
            //                   ^^ curr

            // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
            constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
            constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
            if constexpr(size_t_max < wasm_u32_max)
            {
                // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                if(name_map_length > size_t_max) [[unlikely]]
                {
                    // name map exceeds the platform maximum length (size_t), then pointer addition may have overflow undefined behavior and exit directly.
                    err.emplace_back(curr,
                                     name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                     name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(name_map_length)});
                    return;
                }
            }

            curr = reinterpret_cast<::std::byte const*>(name_map_length_next);

            // [...  section_id name_map_length ...] ...
            // [             safe                  ] unsafe (could be the end)
            //                                       ^^ curr

            if(static_cast<::std::size_t>(end - curr) < static_cast<::std::size_t>(name_map_length)) [[unlikely]]
            {
                // name map exceeds the end - curr, then pointer addition may have overflow undefined behavior and exit directly.
                err.emplace_back(curr, name_err_type_t::illegal_name_map_length, name_err_storage_t{.u32 = name_map_length});
                return;
            }

            // [...  section_id name_map_length ... ... ...] (end)
            // [             safe                  ]... ...] unsafe (could be the end)
            //                                      ^^ curr

            // Subsequent boundary adjustments end to map_end
            auto const map_end{curr + name_map_length};

            // [...  section_id name_map_length ... ... ...] (map_end)
            // [             safe                  ]... ...] unsafe (could be the map_end)
            //                                               ^^ map_end

            switch(section_id)
            {
                    // The previous check is of the total size type and is not relevant to the subsequent checks

                case 0u:
                {
                    // module name

                    if(!ns.module_name.empty()) [[unlikely]]
                    {
                        err.emplace_back(section_id_ptr, name_err_type_t::duplicate_module_name);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [... ] module_name_length ... name_begin ... (map_end)
                    // [safe] unsafe (could be the map_end)
                    //        ^^ curr

                    // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)
                    ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 module_name_length;  // No initialization necessary

                    auto const [module_name_length_next,
                                module_name_length_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                 reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                 ::fast_io::mnp::leb128_get(module_name_length))};

                    if(module_name_length_err != ::fast_io::parse_code::ok) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_module_name_length);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [...  module_name_length ...] name_begin ... (map_end)
                    // [           safe            ] unsafe (could be the map_end)
                    //       ^^ curr

                    // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
                    if constexpr(size_t_max < wasm_u32_max)
                    {
                        // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                        if(module_name_length > size_t_max) [[unlikely]]
                        {
                            err.emplace_back(curr,
                                             name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                             name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(module_name_length)});
                            curr = map_end;
                            // End of current map
                            continue;
                        }
                    }

                    curr = reinterpret_cast<::std::byte const*>(module_name_length_next);

                    // [...  module_name_length ...] name_begin ... (map_end)
                    // [           safe            ] unsafe (could be the map_end)
                    //                               ^^ curr

                    // The length of name cannot be 0
                    if(module_name_length == static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(0u)) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::illegal_module_name_length, name_err_storage_t{.u32 = module_name_length});
                        // End of current map
                        continue;
                    }

                    if(static_cast<::std::size_t>(map_end - curr) < static_cast<::std::size_t>(module_name_length)) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::illegal_module_name_length, name_err_storage_t{.u32 = module_name_length});
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [...  module_name_length ... name_begin ...] (map_end)
                    // [                  safe                    ] unsafe (could be the map_end)
                    //                              ^^ curr

                    auto const module_name_begin{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                    // [...  module_name_length ... name_begin ...] (map_end)
                    // [                  safe                    ] unsafe (could be the map_end)
                    //                              ^^ module_name_begin

                    curr += module_name_length;

                    // [...  module_name_length ... name_begin ...] (map_end)
                    // [                  safe                    ] unsafe (could be the map_end)
                    //                                              ^^ curr

                    auto const module_name_end{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                    // [...  module_name_length ... name_begin ...] (map_end)
                    // [                  safe                    ] unsafe (could be the map_end)
                    //                                              ^^ module_name_end

                    ::fast_io::u8string_view const module_name_tmp{module_name_begin, static_cast<::std::size_t>(module_name_end - module_name_begin)};

                    // Check if it is legal utf-8

                    auto const [utf8pos, utf8err]{
                        ::uwvm2::utils::utf::check_legal_utf8_unchecked<::uwvm2::utils::utf::utf8_specification::utf8_rfc3629_and_zero_illegal>(
                            module_name_tmp.cbegin(),
                            module_name_tmp.cend())};

                    if(utf8err != ::uwvm2::utils::utf::utf_error_code::success) [[unlikely]]
                    {
                        err.emplace_back(reinterpret_cast<::std::byte const*>(utf8pos),
                                         name_err_type_t::illegal_utf8_sequence,
                                         name_err_storage_t{.u32 = static_cast<::std::uint_least32_t>(utf8err)});
                        curr = map_end;
                        // End of current paragraph
                        continue;
                    }

                    // The value is set here in the
                    ns.module_name = module_name_tmp;

                    if(curr != map_end) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_data_exists, name_err_storage_t{.err_uz = static_cast<::std::size_t>(map_end - curr)});
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    break;
                }
                case 1u:
                {
                    // func name

                    if(!ns.function_name.empty()) [[unlikely]]
                    {
                        err.emplace_back(section_id_ptr, name_err_type_t::duplicate_function_name);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [... ] name_count ... (map_end)
                    // [safe] unsafe (could be the map_end)
                    //        ^^ curr

                    ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 name_count;  // number of naming in names
                    auto const [name_count_next, name_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                          reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                          ::fast_io::mnp::leb128_get(name_count))};

                    if(name_count_err != ::fast_io::parse_code::ok) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_function_name_count);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [...  name_count ...] ... (map_end)
                    // [       safe        ] unsafe (could be the map_end)
                    //       ^^ curr

                    // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
                    if constexpr(size_t_max < wasm_u32_max)
                    {
                        // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                        if(name_count > size_t_max) [[unlikely]]
                        {
                            err.emplace_back(curr,
                                             name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                             name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(name_count)});
                            curr = map_end;
                            // End of current map
                            continue;
                        }
                    }

                    curr = reinterpret_cast<::std::byte const*>(name_count_next);

                    // [...  name_count ...] ... (map_end)
                    // [       safe        ] unsafe (could be the map_end)
                    //                       ^^ curr

                    bool ct_1{};

                    for(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 name_counter{}; name_counter != name_count; ++name_counter)
                    {
                        // [... ] func_idx ... name_len ... ... (map_end)
                        // [safe] unsafe (could be the map_end)
                        //        ^^ curr

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_index;
                        auto const [func_index_next, func_index_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                              ::fast_io::mnp::leb128_get(func_index))};

                        // There is no need to do any checking here as it is map.

                        if(func_index_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::invalid_function_index);
                            // Encountered error leb128, could not continue parsing, don't know if there is a problem with subsequent bytes
                            curr = map_end;
                            // End of current map
                            // Implementation of ct_1 via state variables
                            ct_1 = true;
                            break;
                        }

                        // [...  func_idx ...] name_len ... ... (map_end)
                        // [        safe     ] unsafe (could be the map_end)
                        //       ^^ curr

                        auto const func_index_ptr{curr};

                        curr = reinterpret_cast<::std::byte const*>(func_index_next);

                        // [...  func_idx ...] name_len ... ... (map_end)
                        // [        safe     ] unsafe (could be the map_end)
                        //                     ^^ curr

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_name_length;
                        auto const [func_name_length_next,
                                    func_name_length_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                   reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                   ::fast_io::mnp::leb128_get(func_name_length))};

                        // There is no need to do any checking here as it is map.

                        if(func_name_length_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::invalid_function_name_length);
                            // Encountered error leb128, could not continue parsing, don't know if there is a problem with subsequent bytes
                            curr = map_end;
                            // End of current map
                            // Implementation of ct_1 via state variables
                            ct_1 = true;
                            break;
                        }

                        // [...  func_idx ... name_len ...] ... (map_end)
                        // [             safe             ] unsafe (could be the map_end)
                        //                    ^^ curr

                        if constexpr(size_t_max < wasm_u32_max)
                        {
                            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                            if(func_name_length > size_t_max) [[unlikely]]
                            {
                                err.emplace_back(curr,
                                                 name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                                 name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(func_name_length)});
                                curr = map_end;
                                // End of current map
                                // Implementation of ct_1 via state variables
                                ct_1 = true;
                                break;
                            }
                        }

                        curr = reinterpret_cast<::std::byte const*>(func_name_length_next);

                        // [...  func_idx ... name_len ...] ... (map_end)
                        // [             safe             ] unsafe (could be the map_end)
                        //                                  ^^ curr

                        // The length of name cannot be 0
                        if(func_name_length == static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(0u)) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::illegal_function_name_length, name_err_storage_t{.u32 = func_name_length});
                            // End of current paragraph
                            continue;
                        }

                        if(static_cast<::std::size_t>(map_end - curr) < static_cast<::std::size_t>(func_name_length)) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::illegal_function_name_length, name_err_storage_t{.u32 = func_name_length});
                            curr = map_end;
                            // End of current map
                            // Implementation of ct_1 via state variables
                            ct_1 = true;
                            break;
                        }

                        // [...  func_idx ... name_len ... name ...] (map_end)
                        // [                  safe                 ] unsafe (could be the map_end)
                        //                                 ^^ curr

                        auto const func_name_begin{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                        // [...  func_idx ... name_len ... name ...] (map_end)
                        // [                  safe                 ] unsafe (could be the map_end)
                        //                                 ^^ func_name_begin

                        curr += func_name_length;

                        // [...  func_idx ... name_len ... name ...] (map_end)
                        // [                  safe                 ] unsafe (could be the map_end)
                        //                                           ^^ curr

                        auto const func_name_end{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                        // [...  func_idx ... name_len ... name ...] (map_end)
                        // [                  safe                 ] unsafe (could be the map_end)
                        //                                           ^^ func_name_end

                        ::fast_io::u8string_view const func_name_tmp{func_name_begin, static_cast<::std::size_t>(func_name_end - func_name_begin)};

                        // Subsequent parsing will not directly skip the parsing of this map.

                        if(ns.function_name.contains(func_index)) [[unlikely]]
                        {
                            err.emplace_back(func_index_ptr, name_err_type_t::duplicate_func_idx, name_err_storage_t{.u32 = func_index});
                            curr = map_end;
                            // End of current paragraph
                            continue;
                        }

                        // Check if it is legal utf-8

                        auto const [utf8pos, utf8err]{
                            ::uwvm2::utils::utf::check_legal_utf8_unchecked<::uwvm2::utils::utf::utf8_specification::utf8_rfc3629_and_zero_illegal>(
                                func_name_tmp.cbegin(),
                                func_name_tmp.cend())};

                        if(utf8err != ::uwvm2::utils::utf::utf_error_code::success) [[unlikely]]
                        {
                            err.emplace_back(reinterpret_cast<::std::byte const*>(utf8pos),
                                             name_err_type_t::illegal_utf8_sequence,
                                             name_err_storage_t{.u32 = static_cast<::std::uint_least32_t>(utf8err)});
                            curr = map_end;
                            // End of current paragraph
                            continue;
                        }

                        ns.function_name.emplace(func_index, func_name_tmp);
                    }

                    if(ct_1) [[unlikely]]
                    {
                        // End of current map
                        // ct_1 is just a state variable that only controls the jumps
                        // already set "curr = map_end"
                        continue;
                    }

                    if(curr != map_end) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_data_exists, name_err_storage_t{.err_uz = static_cast<::std::size_t>(map_end - curr)});
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    break;
                }
                case 2u:
                {
                    // local name

                    if(!ns.code_local_name.empty()) [[unlikely]]
                    {
                        err.emplace_back(section_id_ptr, name_err_type_t::duplicate_local_name);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [... ] local_count ... ... (map_end)
                    // [safe] unsafe (could be the map_end)
                    //        ^^ curr

                    ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 local_count;  // number of naming in names
                    auto const [local_count_next, local_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                            ::fast_io::mnp::leb128_get(local_count))};

                    if(local_count_err != ::fast_io::parse_code::ok) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_local_count);
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    // [...  local_count ...] ... (map_end)
                    // [        safe        ] unsafe (could be the map_end)
                    //       ^^ curr

                    // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
                    if constexpr(size_t_max < wasm_u32_max)
                    {
                        // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                        if(local_count > size_t_max) [[unlikely]]
                        {
                            err.emplace_back(curr,
                                             name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                             name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(local_count)});
                            curr = map_end;
                            // End of current map
                            continue;
                        }
                    }

                    curr = reinterpret_cast<::std::byte const*>(local_count_next);
                    // [...  local_count ...] ... (map_end)
                    // [        safe        ] unsafe (could be the map_end)
                    //                        ^^ curr

                    bool ct_1{};

                    for(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 local_counter{}; local_counter != local_count; ++local_counter)
                    {
                        // [... ] function_index ... ... (map_end)
                        // [safe] unsafe (could be the map_end)
                        //        ^^ curr

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 function_index;  // number of naming in names
                        auto const [function_index_next, function_index_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                                      reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                                      ::fast_io::mnp::leb128_get(function_index))};

                        if(function_index_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::invalid_function_index);
                            curr = map_end;
                            // End of current map
                            ct_1 = true;
                            break;
                        }

                        // [...  function_index ...] ... (map_end)
                        // [         safe          ] unsafe (could be the map_end)
                        //       ^^ curr

                        auto const function_index_ptr{curr};

                        curr = reinterpret_cast<::std::byte const*>(function_index_next);

                        // [...  function_index ...] function_local_count ... (map_end)
                        // [         safe          ] unsafe (could be the map_end)
                        //                           ^^ curr

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 function_local_count;  // number of naming in names
                        auto const [function_local_count_next,
                                    function_local_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                       reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                       ::fast_io::mnp::leb128_get(function_local_count))};

                        if(function_local_count_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.emplace_back(curr, name_err_type_t::invalid_function_local_count);
                            curr = map_end;
                            // End of current map
                            ct_1 = true;
                            break;
                        }

                        // [...  function_index ... function_local_count ...] ... (map_end)
                        // [                      safe                      ] unsafe (could be the map_end)
                        //                          ^^ curr

                        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
                        if constexpr(size_t_max < wasm_u32_max)
                        {
                            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                            if(function_local_count > size_t_max) [[unlikely]]
                            {
                                err.emplace_back(curr,
                                                 name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                                 name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(function_local_count)});
                                curr = map_end;
                                // End of current map
                                ct_1 = true;
                                break;
                            }
                        }

                        curr = reinterpret_cast<::std::byte const*>(function_local_count_next);

                        // [...  function_index ... function_local_count ...] ... (map_end)
                        // [                      safe                      ] unsafe (could be the map_end)
                        //                                                    ^^ curr

                        ::std::map<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32, ::fast_io::u8string_view> ns_code_local_name_function_index{};

                        bool ct_2{};

                        for(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 function_local_counter{}; function_local_counter != function_local_count;
                            ++function_local_counter)
                        {
                            // [... ] function_local_index ... ... (map_end)
                            // [safe] unsafe (could be the map_end)
                            //        ^^ curr

                            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 function_local_index;  // number of naming in names
                            auto const [function_local_index_next,
                                        function_local_index_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                           reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                           ::fast_io::mnp::leb128_get(function_local_index))};

                            if(function_local_index_err != ::fast_io::parse_code::ok) [[unlikely]]
                            {
                                err.emplace_back(curr, name_err_type_t::invalid_function_local_index);
                                curr = map_end;
                                // End of current map
                                ct_2 = true;
                                break;
                            }

                            // [...  function_local_index ...] ... (map_end)
                            // [            safe             ] unsafe (could be the map_end)
                            //       ^^ curr

                            auto const function_local_index_ptr{curr};

                            curr = reinterpret_cast<::std::byte const*>(function_local_index_next);

                            // [...  function_local_index ...] function_local_name_length ... ... (map_end)
                            // [            safe             ] unsafe (could be the map_end)
                            //                                 ^^ curr

                            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 function_local_name_length;  // number of naming in names
                            auto const [function_local_name_length_next,
                                        function_local_name_length_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                                 reinterpret_cast<char8_t_const_may_alias_ptr>(map_end),
                                                                                                 ::fast_io::mnp::leb128_get(function_local_name_length))};

                            if(function_local_name_length_err != ::fast_io::parse_code::ok) [[unlikely]]
                            {
                                err.emplace_back(curr, name_err_type_t::invalid_function_local_name_length);
                                curr = map_end;
                                // End of current map
                                ct_2 = true;
                                break;
                            }

                            // [...  function_local_index ... function_local_name_length ...] ... (map_end)
                            // [                             safe                           ] unsafe (could be the map_end)
                            //                                ^^ curr

                            if constexpr(size_t_max < wasm_u32_max)
                            {
                                // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                                if(function_local_name_length > size_t_max) [[unlikely]]
                                {
                                    err.emplace_back(curr,
                                                     name_err_type_t::size_exceeds_the_maximum_value_of_size_t,
                                                     name_err_storage_t{.u64 = static_cast<::std::uint_least64_t>(function_local_name_length)});
                                    curr = map_end;
                                    // End of current map
                                    // Implementation of ct_2 via state variables
                                    ct_2 = true;
                                    break;
                                }
                            }

                            curr = reinterpret_cast<::std::byte const*>(function_local_name_length_next);

                            // [...  function_local_index ... function_local_name_length ...] ... (map_end)
                            // [                             safe                           ] unsafe (could be the map_end)
                            //                                                                ^^ curr

                            // The length of name cannot be 0
                            if(function_local_name_length == static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(0u)) [[unlikely]]
                            {
                                err.emplace_back(curr,
                                                 name_err_type_t::illegal_function_local_name_length,
                                                 name_err_storage_t{.u32 = function_local_name_length});
                                // End of current paragraph
                                continue;
                            }

                            if(static_cast<::std::size_t>(map_end - curr) < static_cast<::std::size_t>(function_local_name_length)) [[unlikely]]
                            {
                                err.emplace_back(curr,
                                                 name_err_type_t::illegal_function_local_name_length,
                                                 name_err_storage_t{.u32 = function_local_name_length});
                                curr = map_end;
                                // End of current map
                                // Implementation of ct_2 via state variables
                                ct_2 = true;
                                break;
                            }

                            // [...  function_local_index ... function_local_name_length ... name ...] (map_end)
                            // [                             safe                                    ] unsafe (could be the map_end)
                            //                                                                ^^ curr

                            auto const function_local_name_begin{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                            // [...  function_local_index ... function_local_name_length ... name ...] (map_end)
                            // [                             safe                                    ] unsafe (could be the map_end)
                            //                                                                ^^ function_local_name_begin

                            curr += function_local_name_length;

                            // [...  function_local_index ... function_local_name_length ... name ...] (map_end)
                            // [                             safe                                    ] unsafe (could be the map_end)
                            //                                                                         ^^ curr

                            auto const function_local_name_end{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                            // [...  function_local_index ... function_local_name_length ... name ...] (map_end)
                            // [                             safe                                    ] unsafe (could be the map_end)
                            //                                                                         ^^ function_local_name_end

                            ::fast_io::u8string_view const function_local_name_begin_tmp{
                                function_local_name_begin,
                                static_cast<::std::size_t>(function_local_name_end - function_local_name_begin)};

                            // Subsequent parsing will not directly skip the parsing of this map.

                            if(ns_code_local_name_function_index.contains(function_local_index)) [[unlikely]]
                            {
                                err.emplace_back(function_local_index_ptr,
                                                 name_err_type_t::duplicate_code_local_name_function_index,
                                                 name_err_storage_t{.u32 = function_local_index});
                                curr = map_end;
                                // End of current paragraph
                                continue;
                            }

                            // Check if it is legal utf-8

                            auto const [utf8pos, utf8err]{
                                ::uwvm2::utils::utf::check_legal_utf8_unchecked<::uwvm2::utils::utf::utf8_specification::utf8_rfc3629_and_zero_illegal>(
                                    function_local_name_begin_tmp.cbegin(),
                                    function_local_name_begin_tmp.cend())};

                            if(utf8err != ::uwvm2::utils::utf::utf_error_code::success) [[unlikely]]
                            {
                                err.emplace_back(reinterpret_cast<::std::byte const*>(utf8pos),
                                                 name_err_type_t::illegal_utf8_sequence,
                                                 name_err_storage_t{.u32 = static_cast<::std::uint_least32_t>(utf8err)});
                                curr = map_end;
                                // End of current paragraph
                                continue;
                            }

                            ns_code_local_name_function_index.emplace(function_local_index, function_local_name_begin_tmp);
                        }

                        if(ct_2) [[unlikely]]
                        {
                            ct_1 = true;
                            break;
                        }

                        // Subsequent parsing will not directly skip the parsing of this map.

                        if(ns.code_local_name.contains(function_index)) [[unlikely]]
                        {
                            err.emplace_back(function_index_ptr, name_err_type_t::duplicate_code_function_index, name_err_storage_t{.u32 = function_index});
                            curr = map_end;
                            // End of current paragraph
                            continue;
                        }

                        ns.code_local_name.emplace(function_index, ::std::move(ns_code_local_name_function_index));
                    }

                    if(ct_1) [[unlikely]]
                    {
                        // End of current map
                        // ct_1 is just a state variable that only controls the jumps
                        // already set "curr = map_end"
                        continue;
                    }

                    if(curr != map_end) [[unlikely]]
                    {
                        err.emplace_back(curr, name_err_type_t::invalid_data_exists, name_err_storage_t{.err_uz = static_cast<::std::size_t>(map_end - curr)});
                        curr = map_end;
                        // End of current map
                        continue;
                    }

                    break;
                }
#if 0  /// @todo feature
                case 4u:
                {
                    // type name
                    break;
                }
                case 10u:
                {
                    // field name
                    break;
                }
                case 11u:
                {
                    // tag name
                    break;
                }
#endif
                [[unlikely]] default:
                {
                    err.emplace_back(section_id_ptr, name_err_type_t::illegal_section_id, name_err_storage_t{.u8 = section_id});

                    // Jump directly to the next loop
                    curr = map_end;

                    // [...  section_id name_map_length ... ... ...] (end)
                    // [             safe                  ]... ...] unsafe (could be the end)
                    //                                               ^^ curr

                    break;
                }

                    // Subsequent boundary adjustments map_end to end
            }
        }
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
