/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-07
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
import :types;
import :feature_def;
import :type_section;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <bit>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
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
# include "types.h"
# include "feature_def.h"
# include "type_section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct function_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Function"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::function_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_local_function_type<Fs...>> funcs{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_sequence_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                             \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) ||                                                                                    \
     (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)) ||                                                                   \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)) || (defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)))
    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd128
    /// @details    Support:
    ///
    ///             (Little Endian)
    ///             x86_64-sse2, aarch64-neon, loongarch-SX, wasm-wasmsimd128
    ///
    ///             Due to wasm compact text and after actual testing, the number of functions to function type ratio is 50:1, and in most cases the typeidx of
    ///             commonly used types will be small, with 10,000 functions, almost 90% of the typeidx is single byte. So we only do optimization for
    ///             single-byte simd, not for multi-byte simd, multi-byte relies on shuffle and need to check the table, for almost all single-byte performance
    ///             may not be as good as sequential parsing.
    ///
    ///             Cyclic read 16 bytes, encountered greater than 127 through the mask and crtz calculations are handed over to the rest of the 16 bytes in
    ///             order to deal with the contents of the 16 bytes, and so on the start of the next cycle and then continue to read 16 bytes. There is no mask
    ///             when encountered can not handle the situation can not be located through the mask, only from the beginning of the 16 re-read in order.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd128_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

# if 0 
        /// @deprecated Tested to appear multi-byte situations, simd128 has no advantage over regular

        if(type_section_count >= 128uz)
        {
            using u8x16simd [[__gnu__::__vector_size__(16)]] = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte;
            using u8x16arr = ::fast_io::array<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, 16uz>;

            // Presence of multibytes
            // on SSE2, usd __builtin_ia32_pmovmskb128, direct mask processing of the first bit of each byte

            while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
            {
                u8x16simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

                // mask processing of the first bit of each byte
                using c8x16simd [[__gnu__::__vector_size__(16)]] = char;
                auto const upper_mask{static_cast<::std::uint16_t>(__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(simd_vector_str)))};

                if(
#  if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                    __builtin_expect_with_probability(upper_mask == 0u, true, 0.9)
#  else
                    upper_mask == 0u
#  endif
                        )
#  if !(UWVM_HAS_BUILTIN(__builtin_expect_with_probability))
                    [[likely]]
#  endif
                {
                    // The first byte of each byte is 0, so it is straightforward to write

                    func_counter += 16u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    auto const arr{::std::bit_cast<u8x16arr>(simd_vector_str)};
                    for(auto const i: arr) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + i); }

                    // set curr
                    section_curr += 16u;
                }
                else
                {
                    auto const section_curr_end{section_curr + 16u};

                    auto const byte_need_reed{static_cast<unsigned>(::std::countr_zero(upper_mask))};

#  if __has_cpp_attribute(assume)
                    [[assume(byte_need_reed < 16u)]];
#  endif

                    func_counter += byte_need_reed;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    auto const arr{::std::bit_cast<u8x16arr>(simd_vector_str)};
                    auto const arr_write_end{arr.cbegin() + byte_need_reed};

                    for(auto ic{arr.cbegin()}; ic != arr_write_end; ++ic) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + *ic); }

                    section_curr += byte_need_reed;

                    while(section_curr < section_curr_end)
                    {
                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

                        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

                        auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                        ::fast_io::mnp::leb128_get(typeidx))};

                        if(typeidx_err != ::fast_io::parse_code::ok)
                        {
                            err.err_curr = section_curr;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
                        }

                        // Ensure data is not end of file to inc one
                        if(++func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        if(typeidx >= type_section_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = typeidx;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

                        section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);
                    }
                }
            }
        }
# endif

        if(type_section_count < 128uz)
        {
            // No multibyte cases
            // on SSE2, judge first then __builtin_ia32_pmovmskb128 to figure out if there are any zeros
            // on SSE4, use __builtin_ia32_ptestz128, optimized for all-0 judgments

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
                static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

            while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
            {
                // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
                // [                        safe                             ] unsafe (could be the section_end)
                //                                      ^^ section_curr
                //                                      [   simd_vector_str  ]

                using u8x16simd [[__gnu__::__vector_size__(16)]] = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte;

                u8x16simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

                // It's already a little-endian.

                auto const check_upper{simd_vector_str >= simd_vector_check};

                using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = long long;
                using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = unsigned;
                using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
                using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = unsigned char;
                using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = signed char;

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)
                    __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)
                    __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))
# endif
                        ) [[unlikely]]
                {
                    auto const section_curr_end{section_curr + 16u};

                    // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead of '!='

                    while(section_curr < section_curr_end)
                    {
                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1] ... (outer) ] typeidx2 ...
                        // [     safe    ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        if(++func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

                        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

                        auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                        ::fast_io::mnp::leb128_get(typeidx))};

                        if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
                        }

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] ... (outer) ] typeidx2 ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        // There's a good chance there's an error here.
                        // Or there is a leb redundancy 0
                        if(typeidx >= type_section_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = typeidx;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] typeidx2 ...] ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //                     ^^ section_curr
                    }

                    // [before_section ... | func_count ... typeidx1 ... (15) ... ...  ] typeidxN
                    // [                        safe                                   ] unsafe (could be the section_end)
                    //                                                                   ^^ section_curr
                    //                                      [   simd_vector_str  ] ... ] (Depends on the size of section_curr in relation to section_curr_end)
                }
                else
                {
                    // all are single bytes, so there are 16
                    func_counter += 16u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    using u8x16arr = ::fast_io::array<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, 16uz>;
                    auto const arr{::std::bit_cast<u8x16arr>(simd_vector_str)};
                    for(auto const i: arr) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + i); }

                    section_curr += 16uz;

                    // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                    // [                        safe                             ] unsafe (could be the section_end)
                    //                                                             ^^ section_curr
                }
            }
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX2__) || defined(__ARM_FEATURE_SVE) || defined(__loongarch_asx))
    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd256
    /// @details    Support:
    ///
    ///             (Little Endian)
    ///             mask: x86_64-avx2, aarch64-sve, loongarch-ASX
    ///
    ///             Due to wasm compact text and after actual testing, the number of functions to function type ratio is 50:1, and in most cases the typeidx of
    ///             commonly used types will be small, with 10,000 functions, almost 90% of the typeidx is single byte. So we only do optimization for
    ///             single-byte simd, not for multi-byte simd, multi-byte relies on shuffle and need to check the table, for almost all single-byte performance
    ///             may not be as good as sequential parsing.
    ///
    ///             Cyclic read 32 bytes, encountered greater than 127 then through the mask and crtz calculations are handed over to the sequence of processing
    ///             16 bytes of the rest of the content. The content is divided into two 16-byte, the first 16-byte error loop the contents of the first 16
    ///             bytes and then to the last 16 bytes to restart the cycle ahead of the cycle to read 32. the last 16 bytes will wait for the next cycle to
    ///             read 32 automatically.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd256_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        while(static_cast<::std::size_t>(section_end - section_curr) < 32uz) [[likely]]
        {
            /// @todo
            ::fast_io::fast_terminate();
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX2__) || defined(__ARM_FEATURE_SVE) || defined(__loongarch_asx))
    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd512
    /// @details    Support:
    ///
    ///             (Little Endian)
    ///             mask: x86_64-avx512vbmi
    ///
    ///             Due to wasm compact text and after actual testing, the number of functions to function type ratio is 50:1, and in most cases the typeidx of
    ///             commonly used types will be small, with 10,000 functions, almost 90% of the typeidx is single byte. So we only do optimization for
    ///             single-byte simd, not for multi-byte simd, multi-byte relies on shuffle and need to check the table, for almost all single-byte performance
    ///             may not be as good as sequential parsing.
    ///
    ///             Cyclic read 64 bytes, encountered greater than 127 is calculated by mask and crtz handed over to the order of 32 bytes of the rest of the
    ///             content. The content will be divided into two 32 bytes, the first 32 bytes of the error cycle of the first 32 bytes of the contents of the
    ///             cycle and then to the last 32 bytes to restart the cycle ahead of the cycle to read 64. 32 bytes after the cycle will wait for the next
    ///             cycle to automatically read 32. to avoid the mixing of xmm, ymm, zmm registers, this is not divided into four 16 bytes and then continue to
    ///             calculate the sse instructions
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd512_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        while(static_cast<::std::size_t>(section_end - section_curr) < 64uz) [[likely]]
        {
            /// @todo
            ::fast_io::fast_terminate();
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

    /// @brief Define the handler function for type_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::std::byte const* const sec_id_module_ptr) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse function section (id: 3)"};
#endif
        // Note that section_begin may be equal to section_end
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // get function_section_storage_t from storages
        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(functionsec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8 = functionsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // check has typesec
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};

        if(!typesec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8arr[0] = typesec.section_id;
            err.err_selectable.u8arr[1] = functionsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::forward_dependency_missing;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        functionsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        functionsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        // [before_section ... ] | func_count typeidx1 ...
        // [        safe       ] | unsafe (could be the section_end)
        //                         ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_count{};

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)
        auto const [func_count_next, func_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(func_count))};

        if(func_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_func_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(func_count_err);
        }

        // [before_section ... | func_count ...] typeidx1 ...
        // [             safe                  ] unsafe (could be the section_end)
        //                       ^^ section_curr

        functionsec.funcs.reserve(func_count);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(func_count_next);  // never out of bounds
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // [before_section ... | func_count ...] typeidx1 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        // handle it
#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                             \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) ||                                                                                    \
     (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)) ||                                                                   \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)) || (defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)))
        section_curr = scan_function_section_1b_simd128_impl(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
#else
        section_curr = scan_function_section_sequence_impl(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
#endif
        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        // check function counter match
        if(func_counter != func_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = func_counter;
            err.err_selectable.u32arr[1] = func_count;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_not_match_the_actual_number;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }
}  // namespace uwvm2::parser::wasm::standard::wasm1::features

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
