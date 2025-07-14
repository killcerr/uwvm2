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
 * @date        2025-04-26
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
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
import uwvm2.utils.debug;
# endif
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.standard.wasm1.section;
import uwvm2.parser.wasm.standard.wasm1.opcode;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import :def;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <algorithm>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/string_view.h>
# include <fast_io_dsal/tuple.h>
# include <fast_io_dsal/vector.h>
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    ///
    /// @brief Defining structures or concepts related to wasm1 versions
    ///

    ///////////////////////////
    /// @brief type section ///
    ///////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct type_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    /// @brief Define functions for checking value_type to provide extensibility
    template <typename... Fs>
    concept has_check_typesec_value_type = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                                    ::uwvm2::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> value_type) {
        { define_check_typesec_value_type(sec_adl, value_type) } -> ::std::same_as<bool>;
    };

    /// @brief Define functions to handle type prefix
    template <typename... Fs>
    concept has_type_prefix_handler = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<type_section_storage_t<Fs...>> sec_adl,
                                               ::uwvm2::parser::wasm::standard::wasm1::features::final_type_prefix_t<Fs...> preifx,
                                               ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* section_curr,
                                               ::std::byte const* const section_end,
                                               ::uwvm2::parser::wasm::base::error_impl& err,
                                               ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
                                               ::std::byte const* const prefix_module_ptr) {
        {
            define_type_prefix_handler(sec_adl, preifx, module_storage, section_curr, section_end, err, fs_para, prefix_module_ptr)
        } -> ::std::same_as<::std::byte const*>;
    };

    /////////////////////////////
    /// @brief import section ///
    /////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct import_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    /// @deprecated The wasm standard states that the standard sections follow a sequence, which can never happen
#if 0
    /// @brief Define functions for define_imported_and_defined_exceeding_checker
    template <typename... Fs>
    concept has_imported_and_defined_exceeding_checker = requires(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<::uwvm2::parser::wasm::standard::wasm1::features::final_extern_type_t<Fs...>> extern_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_import_type<Fs...> const*> const* importdesc_begin,
        ::std::byte const* section_curr,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        {
            define_imported_and_defined_exceeding_checker(sec_adl, extern_adl, module_storage, importdesc_begin, section_curr, err, fs_para)
        } -> ::std::same_as<void>;
    };
#endif

    /// @brief Define functions for handle extern_prefix
    template <typename... Fs>
    concept has_extern_prefix_imports_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::final_import_type<Fs...>& fit,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            {
                define_extern_prefix_imports_handler(sec_adl, fit.imports, module_storage, section_curr, section_end, err, fs_para)
            } -> ::std::same_as<::std::byte const*>;
        };

    /// @brief Defining structures or concepts related to wasm versions
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct wasm1_final_extern_type
    {
        union storage_t
        {
            ::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const* function;
            static_assert(::std::is_trivially_copyable_v<decltype(function)> && ::std::is_trivially_destructible_v<decltype(function)>);

            ::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<Fs...> table;
            static_assert(::std::is_trivially_copyable_v<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<Fs...>> &&
                          ::std::is_trivially_destructible_v<::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<Fs...>>);

            ::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...> memory;
            static_assert(::std::is_trivially_copyable_v<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...>> &&
                          ::std::is_trivially_destructible_v<::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...>>);

            ::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<Fs...> global;
            static_assert(::std::is_trivially_copyable_v<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<Fs...>> &&
                          ::std::is_trivially_destructible_v<::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<Fs...>>);
        } storage;

        ::uwvm2::parser::wasm::standard::wasm1::type::external_types type{};
    };

    /// @brief Define functions for handle imports table
    template <typename... Fs>
    concept has_extern_imports_table_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_extern_type<Fs...>& fit_imports,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            {
                extern_imports_table_handler(sec_adl, fit_imports.storage.table, module_storage, section_curr, section_end, err, fs_para)
            } -> ::std::same_as<::std::byte const*>;
        };

    /// @brief Define functions for handle imports memory
    template <typename... Fs>
    concept has_extern_imports_memory_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_extern_type<Fs...>& fit_imports,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            {
                extern_imports_memory_handler(sec_adl, fit_imports.storage.memory, module_storage, section_curr, section_end, err, fs_para)
            } -> ::std::same_as<::std::byte const*>;
        };

    /// @brief Define functions for handle imports global
    template <typename... Fs>
    concept has_extern_imports_global_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<import_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_final_extern_type<Fs...>& fit_imports,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            {
                extern_imports_global_handler(sec_adl, fit_imports.storage.global, module_storage, section_curr, section_end, err, fs_para)
            } -> ::std::same_as<::std::byte const*>;
        };

    /// @brief Define functions for checking imports and exports name
    template <typename... Fs>
    concept can_check_import_export_text_format = requires(::uwvm2::parser::wasm::standard::wasm1::features::final_text_format_wapper<Fs...> adl,
                                                           ::std::byte const* begin,
                                                           ::std::byte const* end,
                                                           ::uwvm2::parser::wasm::base::error_impl& err) {
        { check_import_export_text_format(adl, begin, end, err) } -> ::std::same_as<void>;
    };

    ///////////////////////////////
    /// @brief function section ///
    ///////////////////////////////

    struct function_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    enum class vectypeidx_minimize_storage_mode : unsigned
    {
        null,
        u8_view,
        u8_vector,
        u16_vector,
        u32_vector
    };

    struct typeidx_u8_view_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8 const* begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8 const* end{};
    };

    struct vectypeidx_minimize_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::std::size_t sizeof_vectypeidx_minimize_storage_u{
            ::uwvm2::parser::wasm::concepts::operation::get_union_size<typeidx_u8_view_t,
                                                                       ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>,
                                                                       ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>,
                                                                       ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>>()};

        union vectypeidx_minimize_storage_u UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
        {
            typeidx_u8_view_t typeidx_u8_view;
            static_assert(::std::is_trivially_copyable_v<typeidx_u8_view_t> && ::std::is_trivially_destructible_v<typeidx_u8_view_t>);

            // Self-control of the life cycle
            ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8> typeidx_u8_vector;
            static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<decltype(typeidx_u8_vector)> &&
                          ::fast_io::freestanding::is_zero_default_constructible_v<decltype(typeidx_u8_vector)>);

            // Self-control of the life cycle
            ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16> typeidx_u16_vector;
            static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<decltype(typeidx_u16_vector)> &&
                          ::fast_io::freestanding::is_zero_default_constructible_v<decltype(typeidx_u16_vector)>);

            // Self-control of the life cycle
            ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32> typeidx_u32_vector;
            static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<decltype(typeidx_u32_vector)> &&
                          ::fast_io::freestanding::is_zero_default_constructible_v<decltype(typeidx_u32_vector)>);

            // Full occupancy is used to initialize the union, set the union to all zero.
            [[maybe_unused]] ::std::byte vectypeidx_minimize_storage_u_reserve[sizeof_vectypeidx_minimize_storage_u]{};

            // destructor of 'vectypeidx_minimize_storage_u' is implicitly deleted because variant field 'typeidx_u8_vector' has a non-trivial destructor
            inline constexpr ~vectypeidx_minimize_storage_u() {}

            // The release of fast_io::vectors is managed by struct vectypeidx_minimize_storage_t, there is no issue of raii resources being unreleased.
        };

        static_assert(sizeof(vectypeidx_minimize_storage_u) == sizeof_vectypeidx_minimize_storage_u,
                      "sizeof_vectypeidx_minimize_storage_u not equal to sizeof_vectypeidx_minimize_storage_u");

        vectypeidx_minimize_storage_u storage{};
        vectypeidx_minimize_storage_mode mode{};

        inline constexpr vectypeidx_minimize_storage_t() noexcept = default;  // all-zero

        inline constexpr vectypeidx_minimize_storage_t(vectypeidx_minimize_storage_t const& other) noexcept : mode{other.mode}
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    // trivial, placement new (regulate)
                    ::new(::std::addressof(this->storage.typeidx_u8_view)) decltype(this->storage.typeidx_u8_view){other.storage.typeidx_u8_view};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    // copy constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u8_vector)) decltype(this->storage.typeidx_u8_vector){other.storage.typeidx_u8_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    // copy constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u16_vector)) decltype(this->storage.typeidx_u16_vector){other.storage.typeidx_u16_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    // copy constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u32_vector)) decltype(this->storage.typeidx_u32_vector){other.storage.typeidx_u32_vector};
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        inline constexpr vectypeidx_minimize_storage_t(vectypeidx_minimize_storage_t&& other) noexcept : mode{other.mode}
        {
            // Since fast_io::vector satisfies both is_trivially_copyable_or_relocatable and is_zero_default_constructible, it is possible to do this

            // There is no need to set other.mode here, as it is a move, and if you want to modify other.mode you need to destruct the type on other

            // move data
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    // trivial, placement new (regulate)
                    ::new(::std::addressof(this->storage.typeidx_u8_view)) decltype(this->storage.typeidx_u8_view){::std::move(other.storage.typeidx_u8_view)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    // move constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u8_vector)) decltype(this->storage.typeidx_u8_vector){
                        ::std::move(other.storage.typeidx_u8_vector)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    // move constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u16_vector)) decltype(this->storage.typeidx_u16_vector){
                        ::std::move(other.storage.typeidx_u16_vector)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    // move constructor, placement new
                    ::new(::std::addressof(this->storage.typeidx_u32_vector)) decltype(this->storage.typeidx_u32_vector){
                        ::std::move(other.storage.typeidx_u32_vector)};
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        inline constexpr vectypeidx_minimize_storage_t& operator= (vectypeidx_minimize_storage_t const& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // Prevent type puns, must destruct union
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    // The trivial destructor type does not require any destructors.
                    static_assert(::std::is_trivially_destructible_v<decltype(this->storage.typeidx_u8_view)>);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u8_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u16_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u32_vector));
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            this->mode = other.mode;

            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    ::new(::std::addressof(this->storage.typeidx_u8_view)) decltype(this->storage.typeidx_u8_view){other.storage.typeidx_u8_view};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u8_vector)) decltype(this->storage.typeidx_u8_vector){other.storage.typeidx_u8_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u16_vector)) decltype(this->storage.typeidx_u16_vector){other.storage.typeidx_u16_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u32_vector)) decltype(this->storage.typeidx_u32_vector){other.storage.typeidx_u32_vector};
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            return *this;
        }

        inline constexpr vectypeidx_minimize_storage_t& operator= (vectypeidx_minimize_storage_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // Prevent type puns, must destruct union
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    static_assert(::std::is_trivially_destructible_v<decltype(this->storage.typeidx_u8_view)>);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u8_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u16_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u32_vector));
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            // Since fast_io::vector satisfies both is_trivially_copyable_or_relocatable and is_zero_default_constructible, it is possible to do this

            this->mode = other.mode;

            // There is no need to set other.mode here, as it is a move, and if you want to modify other.mode you need to destruct the type on other

            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    // do nothing
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    ::new(::std::addressof(this->storage.typeidx_u8_view)) decltype(this->storage.typeidx_u8_view){::std::move(other.storage.typeidx_u8_view)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u8_vector)) decltype(this->storage.typeidx_u8_vector){
                        ::std::move(other.storage.typeidx_u8_vector)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u16_vector)) decltype(this->storage.typeidx_u16_vector){
                        ::std::move(other.storage.typeidx_u16_vector)};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u32_vector)) decltype(this->storage.typeidx_u32_vector){
                        ::std::move(other.storage.typeidx_u32_vector)};
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            return *this;
        }

        inline constexpr ~vectypeidx_minimize_storage_t()
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    // Multiple destructuring is ub in the standard, so mundane types don't need to do any operations
                    // this->storage.typeidx_u8_view = {};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u8_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u16_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u32_vector));
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            // No need to set this mode, multiple destructs ub
        }

        inline constexpr void clear_destroy() noexcept
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    this->storage.typeidx_u8_view = {};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    // clear_destroy equivalent to destructuring
                    this->storage.typeidx_u8_vector.clear_destroy();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    // clear_destroy equivalent to destructuring
                    this->storage.typeidx_u16_vector.clear_destroy();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    // clear_destroy equivalent to destructuring
                    this->storage.typeidx_u32_vector.clear_destroy();
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        inline constexpr void clear() noexcept
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    this->storage.typeidx_u8_view = {};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector.clear();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector.clear();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector.clear();
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        inline constexpr void change_mode(vectypeidx_minimize_storage_mode mode) noexcept
        {
            // chang mode need destroy first

            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    // Multiple destructuring is ub in the standard, so mundane types don't need to do any operations
                    // this->storage.typeidx_u8_view = {};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u8_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u16_vector));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::std::destroy_at(::std::addressof(this->storage.typeidx_u32_vector));
                    break;
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }

            this->mode = mode;

            // Because the fast_io vector is constructed with all zeros, there is no need to do any constructor operations
            static_assert(::std::is_trivially_copyable_v<decltype(this->storage.typeidx_u8_view)> &&
                          ::std::is_trivially_destructible_v<decltype(this->storage.typeidx_u8_view)>);
            static_assert(::fast_io::freestanding::is_zero_default_constructible_v<decltype(this->storage.typeidx_u8_vector)>);
            static_assert(::fast_io::freestanding::is_zero_default_constructible_v<decltype(this->storage.typeidx_u16_vector)>);
            static_assert(::fast_io::freestanding::is_zero_default_constructible_v<decltype(this->storage.typeidx_u32_vector)>);

            // The union behavior is equivalent to the constructor
            ::std::memset(this->storage.vectypeidx_minimize_storage_u_reserve, 0, sizeof(this->storage.vectypeidx_minimize_storage_u_reserve));
        }

        inline constexpr ::std::size_t size() const noexcept
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::fast_io::fast_terminate();
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    return static_cast<::std::size_t>(this->storage.typeidx_u8_view.end - this->storage.typeidx_u8_view.begin);
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    return this->storage.typeidx_u8_vector.size();
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    return this->storage.typeidx_u16_vector.size();
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    return this->storage.typeidx_u32_vector.size();
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }

        inline constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 index_unchecked(::std::size_t sz) const noexcept
        {
            switch(this->mode)
            {
                [[unlikely]] case vectypeidx_minimize_storage_mode::null:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::fast_io::fast_terminate();
                }
                case vectypeidx_minimize_storage_mode::u8_view:
                {
                    return static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(this->storage.typeidx_u8_view.begin[sz]);
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    return static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(this->storage.typeidx_u8_vector.index_unchecked(sz));
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    return static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(this->storage.typeidx_u16_vector.index_unchecked(sz));
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    return this->storage.typeidx_u32_vector.index_unchecked(sz);
                }
                [[unlikely]] default:
                {
#if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                    ::uwvm2::utils::debug::trap_and_inform_bug_pos();
#endif
                    ::std::unreachable();
                }
            }
        }
    };

    ////////////////////////////
    /// @brief table section ///
    ////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct table_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    /// @brief Define functions for handle table section table
    template <typename... Fs>
    concept has_table_section_table_handler = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<table_section_storage_t<Fs...>> sec_adl,
                                                       ::uwvm2::parser::wasm::standard::wasm1::features::final_table_type<Fs...>& table_r,
                                                       ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                                       ::std::byte const* section_curr,
                                                       ::std::byte const* const section_end,
                                                       ::uwvm2::parser::wasm::base::error_impl& err,
                                                       ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        { table_section_table_handler(sec_adl, table_r, module_storage, section_curr, section_end, err, fs_para) } -> ::std::same_as<::std::byte const*>;
    };

    /////////////////////////////
    /// @brief memory section ///
    /////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct memory_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    /// @brief Define functions for handle memory section memory
    template <typename... Fs>
    concept has_memory_section_memory_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<memory_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::final_memory_type<Fs...>& memory_r,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            { memory_section_memory_handler(sec_adl, memory_r, module_storage, section_curr, section_end, err, fs_para) } -> ::std::same_as<::std::byte const*>;
        };

    /////////////////////////////
    /// @brief global section ///
    /////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct global_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    /// @brief Define functions for handle global section global
    template <typename... Fs>
    concept has_global_section_global_handler =
        requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<global_section_storage_t<Fs...>> sec_adl,
                 ::uwvm2::parser::wasm::standard::wasm1::features::final_global_type<Fs...>& global_r,
                 ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                 ::std::byte const* section_curr,
                 ::std::byte const* const section_end,
                 ::uwvm2::parser::wasm::base::error_impl& err,
                 ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
            { global_section_global_handler(sec_adl, global_r, module_storage, section_curr, section_end, err, fs_para) } -> ::std::same_as<::std::byte const*>;
        };

    /////////////////////////////
    /// @brief export section ///
    /////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct export_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    union wasm1_final_export_storage_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_idx;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 table_idx;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 memory_idx;
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 global_idx;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct wasm1_final_export_type
    {
        wasm1_final_export_storage_t storage{};
        ::uwvm2::parser::wasm::standard::wasm1::type::external_types type{};
    };

    template <typename... Fs>
    concept has_handle_export_index = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<export_section_storage_t<Fs...>> sec_adl,
                                               ::uwvm2::parser::wasm::standard::wasm1::features::final_export_type_t<Fs...>& fwet,
                                               ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* section_curr,
                                               ::std::byte const* const section_end,
                                               ::uwvm2::parser::wasm::base::error_impl& err,
                                               ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        {
            define_handler_export_index(sec_adl, fwet.storage, fwet.type, module_storage, section_curr, section_end, err, fs_para)
        } -> ::std::same_as<::std::byte const*>;
    };

    //////////////////////////////
    /// @brief element section ///
    //////////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct element_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    struct wasm1_elem_expr_t
    {
        // Expressions are encoded by their instruction sequence terminated with an explicit 0x0B opcode for end.
        inline static constexpr auto end_opcode{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0x0Bu)};

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* end{};  // The pointer to end is after 0x0b.
    };

    struct wasm1_elem_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 table_idx{};
        wasm1_elem_expr_t expr{};
        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32> vec_funcidx{};
    };
}

// Subsequent specifications of union must include this information, so it has to be declared here.
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::wasm1_elem_storage_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::wasm1_elem_storage_t>
    {
        inline static constexpr bool value = true;
    };
}

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    enum class wasm1_element_type_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {
        tableidx = 0u,
    };

    template <typename... Fs>  // Fs is used as an extension to an existing type, but does not extend the type
    struct wasm1_element_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::std::size_t sizeof_storage_u{sizeof(::uwvm2::parser::wasm::standard::wasm1::features::wasm1_elem_storage_t)};

        union storage_u UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
        {
            // This type can be initialized with all zeros
            ::uwvm2::parser::wasm::standard::wasm1::features::wasm1_elem_storage_t table_idx;
            static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<decltype(table_idx)> &&
                          ::fast_io::freestanding::is_zero_default_constructible_v<decltype(table_idx)>);

            // Full occupancy is used to initialize the union, set the union to all zero.
            [[maybe_unused]] ::std::byte sizeof_storage_u_reserve[sizeof_storage_u]{};

            // destructor of 'storage_u' is implicitly deleted because variant field 'typeidx_u8_vector' has a non-trivial destructor
            inline constexpr ~storage_u() {}

            // The release of table_idx is managed by struct wasm1_element_t, there is no issue of raii resources being unreleased.
        } storage;

        static_assert(sizeof(storage_u) == sizeof_storage_u, "sizeof(storage_t) not equal to sizeof_storage_u");

        // In wasm1, type stands for table index, which conceptually can be any value, but since the standard specifies only 1 table, it can only be 0. Here
        // union does not need to make any type-safe judgments since there is only one type.

        wasm1_element_type_t type{};

        inline constexpr wasm1_element_t() noexcept = default;  // all-zero

        inline constexpr wasm1_element_t(wasm1_element_t const& other) noexcept : type{other.type}
        {
            ::new(::std::addressof(this->storage.table_idx)) decltype(this->storage.table_idx){other.storage.table_idx};
        }

        inline constexpr wasm1_element_t(wasm1_element_t&& other) noexcept : type{other.type}
        {
            ::new(::std::addressof(this->storage.table_idx)) decltype(this->storage.table_idx){::std::move(other.storage.table_idx)};
        }

        inline constexpr wasm1_element_t& operator= (wasm1_element_t const& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // The table_idx type in union is always valid regardless of the value of type.

            this->type = other.type;

            this->storage.table_idx = other.storage.table_idx;

            return *this;
        }

        inline constexpr wasm1_element_t& operator= (wasm1_element_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            // The table_idx type in union is always valid regardless of the value of type.

            this->type = other.type;

            this->storage.table_idx = ::std::move(other.storage.table_idx);

            return *this;
        }

        inline constexpr ~wasm1_element_t() { ::std::destroy_at(::std::addressof(this->storage.table_idx)); }
    };

    template <typename... Fs>
    concept has_handle_element_type = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<element_section_storage_t<Fs...>> sec_adl,
                                               ::uwvm2::parser::wasm::standard::wasm1::features::final_element_type_t<Fs...>& fet,
                                               ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                               ::std::byte const* section_curr,
                                               ::std::byte const* const section_end,
                                               ::uwvm2::parser::wasm::base::error_impl& err,
                                               ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        {
            define_handler_element_type(sec_adl, fet.storage, fet.type, module_storage, section_curr, section_end, err, fs_para)
        } -> ::std::same_as<::std::byte const*>;
    };

    ///////////////////////////
    /// @brief code section ///
    ///////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct code_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    struct code_body_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* code_begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* expr_begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* code_end{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_local_entry_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 count{};
        ::uwvm2::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> type{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct final_wasm_code_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        code_body_t body{};
        ::fast_io::vector<final_local_entry_t<Fs...>> locals{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 all_local_count{};
    };

    /// @brief Define functions for checking value_type to provide extensibility
    template <typename... Fs>
    concept has_check_codesec_value_type = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<code_section_storage_t<Fs...>> sec_adl,
                                                    ::uwvm2::parser::wasm::standard::wasm1::features::final_value_type_t<Fs...> value_type) {
        { define_check_codesec_value_type(sec_adl, value_type) } -> ::std::same_as<bool>;
    };

    ///////////////////////////
    /// @brief data section ///
    ///////////////////////////

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct data_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE;

    struct wasm1_data_expr_t
    {
        // Expressions are encoded by their instruction sequence terminated with an explicit 0x0B opcode for end.
        inline static constexpr auto end_opcode{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::op_basic_type>(0x0Bu)};

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* end{};  // The pointer to end is after 0x0b.
    };

    struct wasm1_data_init_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* begin{};
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const* end{};
    };

    struct wasm1_data_storage_t
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 memory_idx{};
        wasm1_data_expr_t expr{};
        wasm1_data_init_t byte{};
    };

    enum class wasm1_data_type_t : ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32
    {
        memoryidx = 0u,
    };

    template <typename... Fs>  // Fs is used as an extension to an existing type, but does not extend the type
    struct wasm1_data_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        union storage_u
        {
            wasm1_data_storage_t memory_idx;
            static_assert(::std::is_trivially_copyable_v<wasm1_data_storage_t> && ::std::is_trivially_destructible_v<wasm1_data_storage_t>);

        } storage;

        wasm1_data_type_t type{};
    };

    template <typename... Fs>
    concept has_handle_data_type = requires(::uwvm2::parser::wasm::concepts::feature_reserve_type_t<data_section_storage_t<Fs...>> sec_adl,
                                            ::uwvm2::parser::wasm::standard::wasm1::features::final_data_type_t<Fs...>& fet,
                                            ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...>& module_storage,
                                            ::std::byte const* section_curr,
                                            ::std::byte const* const section_end,
                                            ::uwvm2::parser::wasm::base::error_impl& err,
                                            ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para) {
        {
            define_handler_data_type(sec_adl, fet.storage, fet.type, module_storage, section_curr, section_end, err, fs_para)
        } -> ::std::same_as<::std::byte const*>;
    };

    //////////////////////////
    /// @brief Final Check ///
    //////////////////////////

    struct wasm1_final_check;

}  // namespace uwvm2::parser::wasm::standard::wasm1::features

UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_t>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::wasm1_element_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_local_entry_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::final_wasm_code_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::final_wasm_code_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::wasm1_data_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
