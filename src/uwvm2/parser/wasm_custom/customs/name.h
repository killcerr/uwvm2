/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-07-09
 * @copyright   ASHP-1.0 License
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
import uwvm2.utils.utf;
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
# include <uwvm2/utils/utf/impl.h>
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

    inline constexpr void parse_name_storage(name_storage_t & ns, ::std::byte const* const begin, ::std::byte const* const end) noexcept
    {
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
                // If you can't get the length of the name map, then anything that follows may be misplaced and you'll just quit.
                return;
            }

            // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
            constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
            constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
            if constexpr(size_t_max < wasm_u32_max)
            {
                // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                if(name_map_length > size_t_max) [[unlikely]]
                {
                    // name map exceeds the platform maximum length (size_t), then pointer addition may have overflow undefined behavior and exit directly.
                    return;
                }
            }

            // [...  section_id name_map_length ...] ...
            // [             safe                  ] unsafe (could be the end)
            //                   ^^ curr

            curr = reinterpret_cast<::std::byte const*>(name_map_length_next);

            // [...  section_id name_map_length ...] ...
            // [             safe                  ] unsafe (could be the end)
            //                                       ^^ curr

            if(static_cast<::std::size_t>(end - curr) < static_cast<::std::size_t>(name_map_length)) [[unlikely]]
            {
                // name map exceeds the end - curr, then pointer addition may have overflow undefined behavior and exit directly.
                return;
            }

            // [...  section_id name_map_length ... ... ...] (end)
            // [             safe                  ]... ...] unsafe (could be the end)
            //                                      ^^ curr

            auto const map_end{curr + name_map_length};

            // [...  section_id name_map_length ... ... ...] (end)
            // [             safe                  ]... ...] unsafe (could be the end)
            //                                               ^^ curr

            switch(section_id)
            {
                    // The previous check is of the total size type and is not relevant to the subsequent checks

                case 0u:
                {
                    // module name

                    // [... ] module_name_length ... name_begin ... (end)
                    // [safe] unsafe (could be the end)
                    //        ^^ curr

                    // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)
                    ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 module_name_length;  // No initialization necessary

                    auto const [module_name_length_next, module_name_length_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(curr),
                                                                                                          reinterpret_cast<char8_t_const_may_alias_ptr>(end),
                                                                                                          ::fast_io::mnp::leb128_get(module_name_length))};

                    if(module_name_length_err != ::fast_io::parse_code::ok) [[unlikely]]
                    {
                        curr = map_end;
                        continue;
                    }

                    // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
                    if constexpr(size_t_max < wasm_u32_max)
                    {
                        // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
                        if(module_name_length > size_t_max) [[unlikely]]
                        {
                            curr = map_end;
                            continue;
                        }
                    }

                    // [...  module_name_length ...] name_begin ... (end)
                    // [           safe            ] unsafe (could be the end)
                    //       ^^ curr

                    curr = reinterpret_cast<::std::byte const*>(module_name_length_next);

                    // [...  module_name_length ...] name_begin ... (end)
                    // [           safe            ] unsafe (could be the end)
                    //                               ^^ curr

                    if(static_cast<::std::size_t>(end - curr) < static_cast<::std::size_t>(module_name_length)) [[unlikely]]
                    {
                        curr = map_end;
                        continue;
                    }

                    // [...  module_name_length ... name_begin ...] (end)
                    // [                  safe                    ] unsafe (could be the end)
                    //                              ^^ curr

                    auto const module_name_begin{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                    // [...  module_name_length ... name_begin ...] (end)
                    // [                  safe                    ] unsafe (could be the end)
                    //                              ^^ module_name_begin

                    curr += module_name_length;

                    // [...  module_name_length ... name_begin ...] (end)
                    // [                  safe                    ] unsafe (could be the end)
                    //                                              ^^ curr

                    auto const module_name_end{reinterpret_cast<char8_t_const_may_alias_ptr>(curr)};

                    // [...  module_name_length ... name_begin ...] (end)
                    // [                  safe                    ] unsafe (could be the end)
                    //                                              ^^ module_name_end

                    ::fast_io::u8string_view const module_name_tmp{module_name_begin, static_cast<::std::size_t>(module_name_end - module_name_begin)};

                    // Check if it is legal utf-8

                    auto const [utf8pos, utf8err]{
                        ::uwvm2::utils::utf::check_legal_utf8_unchecked<::uwvm2::utils::utf::utf8_specification::utf8_rfc3629_and_zero_illegal>(
                            module_name_tmp.cbegin(),
                            module_name_tmp.cend())};

                    if(utf8err != ::uwvm2::utils::utf::utf_error_code::success) [[unlikely]]
                    {
                        curr = map_end;
                        continue;
                    }

                    ns.module_name = module_name_tmp;

                    break;
                }
                case 1u:
                {
                    // func name
                    break;
                }
                case 2u:
                {
                    // local name
                    break;
                }
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
                [[unlikely]] default:
                {
                    // Jump directly to the next loop
                    curr += name_map_length;

                    // [...  section_id name_map_length ... ... ...] (end)
                    // [             safe                  ]... ...] unsafe (could be the end)
                    //                                               ^^ curr

                    break;
                }
            }
        }
    }
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
