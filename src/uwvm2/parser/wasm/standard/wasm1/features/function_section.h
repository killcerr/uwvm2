/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-07
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
# ifdef UWVM_TIMER
import uwvm2.utils.debug;
# endif
import uwvm2.utils.intrinsics;
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
# if (defined(_MSC_VER) && !defined(__clang__)) && !defined(_KERNEL_MODE) && defined(_M_AMD64)
#  include <emmintrin.h>  // MSVC x86_64-SSE2
# endif
# if (defined(_MSC_VER) && !defined(__clang__)) && !defined(_KERNEL_MODE) && defined(_M_ARM64)
#  include <arm_neon.h>  // MSVC aarch64-NEON
# endif
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
# include <uwvm2/utils/intrinsics/impl.h>
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
    struct function_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Function"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::function_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_t funcs{};
    };

    /// @brief      Convert view to vec
    /// @param      func_counter The correct u8 size has been processed
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* change_u8_1b_view_to_vec(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 7u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // Storing a typeidx takes 1 bytes, change u8view to u8_vector
        functionsec.funcs.change_mode(::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u8_vector);

        // The size comparison between u32 and size_t has already been checked
        functionsec.funcs.storage.typeidx_u8_vector.reserve(static_cast<::std::size_t>(func_count));

        static_assert(sizeof(::std::byte) == sizeof(::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8));

        // Copy the situation that has been processed correctly
        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr, section_curr - func_counter, func_counter);
        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += func_counter;

        // [typeidxbef ...] typeidx1 ... typeidx2 ...
        // [     safe     ] unsafe (could be the section_end)
        //                  ^^ section_curr

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

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

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

    /// @brief      Maximum typeidx in [0, 2^7)
    /// @details    Storing a typeidx takes up 1 bytes, typeidx corresponding uleb128 varies from 1 bytes, linear read/write, no simd optimization
    ///             Sequential scanning, correctly handling all cases of uleb128 u32, allowing up to 5 bytes.
    ///
    ///             According to the characteristics of wasm, wasm-ld will try to avoid redundancy 0 allowed by uleb128, so basically all cases are one byte,
    ///             this time we only need to check whether they are all 1 byte, and no more than typeidx, the total length matches, to meet these points can be
    ///             used directly to view to avoid copying, and you can simd parallel computation and writing. In rare cases, such as the user to modify the
    ///             wasm file, or wasm-ld does not use the least byte representation, encounter redundancy 0, uleb128<u32> will appear to be greater than 1 byte
    ///             in length, but meets the standard, this time it will be allocated space and the previous contents of the memcpy to the space, and then
    ///             continue to read to ensure that the subsequent access to ensure that access can be accessed through the index directly to the This will
    ///             increase the scanning time.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_impl_u8_1b(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 7u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // used directly to u8 view to avoid copying
        functionsec.funcs.mode = ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u8_view;

        // set view begin
        using wasm_u8_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8 const*;
        functionsec.funcs.storage.typeidx_u8_view.begin = reinterpret_cast<wasm_u8_const_may_alias_ptr>(section_curr);

#if (defined(_MSC_VER) && !defined(__clang__)) && !defined(_KERNEL_MODE) && (defined(_M_AMD64) || defined(_M_ARM64))
        /// (Little Endian), MSVC, ::fast_io::intrinsics::simd_vector
        /// x86_64-sse2, aarch64-neon
        /// @todo need check

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

        // use fast_io simd on msvc
        using simd128_t = ::fast_io::intrinsics::simd_vector<::std::uint8_t, 16uz>;

        simd128_t const simd_vector_check_u8x16{simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check,
                                                simd_vector_check};

        while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            simd128_t simd_vector_str;  // No initialization necessary

            simd_vector_str.load(section_curr);

            // It's already a little-endian.

            auto const check_upper{simd_vector_str >= simd_vector_check_u8x16};

            if(!is_all_zeros(check_upper)) [[unlikely]]
            {
                // There are special cases greater than typeidx, which may be legal redundancy 0 or illegal data, all converted to vec to determine.
                return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
            }
            else
            {
                // all are single bytes, so there are 16
                // There is no need to staging func_counter_this_round_end, as the special case no longer exists.
                func_counter += 16u;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                section_curr += 16uz;

                // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }

#elif defined(__LITTLE_ENDIAN__) && (defined(__ARM_FEATURE_SVE) || (defined(UWVM_ENABLE_SME_SVE_STREAM_MODE) && defined(__ARM_FEATURE_SME)))
        /// (Little Endian) Variable Length
        /// mask: aarch64-sve

        // Lambda is an external function call, and error handling needs to be carried out in current function
        bool need_change_u8_1b_view_to_vec{};

        // Support for certain cpu's that don't have SVE but have SME like Apple M4
        //
        // However, for this type of cpu (e.g. Apple M4), the sme module is separated from the cpu,
        // resulting in very high latency when using it (on Apple M4, SME (SVE stream mode) 15x slower than NEON),
        // so llvm for Apple M4 march=native will not start armv8a+sme, but rather use it as armv8a.
        //
        // Here you just need to leave it to llvm's judgment, if the sme unit is done inside the cpu,
        // then llvm's march=native will turn on by itself.
        //
        // You can use macro UWVM_ENABLE_SME_SVE_STREAM_MODE to enable SVE stream mode in SME in the above cpu (e.g. Apple M4).

        [&]
# if (defined(UWVM_ENABLE_SME_SVE_STREAM_MODE) && defined(__ARM_FEATURE_SME)) && !defined(__ARM_FEATURE_SVE)
            __arm_locally_streaming
# endif
            () constexpr UWVM_THROWS -> void
        {
            static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
                static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

            auto const all_one_predicate_reg{::uwvm2::utils::intrinsics::arm_sve::svptrue_b8()};
            auto const svc_sz{::uwvm2::utils::intrinsics::arm_sve::svcntb()};

# if defined(__ARM_NEON) && (UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32) || UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu))
            // When the cpu supports the vector length of sve to be the same as the vector length of neon, since the computations are all on the basic side and
            // read 16 bytes at a time, sve needs to additionally process predicates, resulting in lower throughput, so here it switches back to using neon

            if(svc_sz == 16u)
            {
                using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
                using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
                using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
                using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
                using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
                using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

                while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
                {
                    // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
                    // [                        safe                             ] unsafe (could be the section_end)
                    //                                      ^^ section_curr
                    //                                      [   simd_vector_str  ]

                    u8x16simd simd_vector_str;  // No initialization necessary

                    ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

                    // It's already a little-endian.

                    auto const check_upper{simd_vector_str >= simd_vector_check};

                    if(
#  if UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                    // Only supported by clang
                        __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_upper))
#  elif UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                        __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_upper))
#  else
#   error "missing instructions"
#  endif
                            ) [[unlikely]]
                    {
                        need_change_u8_1b_view_to_vec = true;
                        return;
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

                        section_curr += 16uz;

                        // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                        // [                        safe                             ] unsafe (could be the section_end)
                        //                                                             ^^ section_curr
                    }
                }
            }
            else
# endif
            {
                while(static_cast<::std::size_t>(section_end - section_curr) >= svc_sz) [[likely]]
                {
                    // [before_section ... | func_count ... typeidx1 ... (svc_sz - 1) ...] ...
                    // [                        safe                                     ] unsafe (could be the section_end)
                    //                                      ^^ section_curr
                    //                                      [       simd_vector_str      ]

                    using uint8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint8_t const*;

                    // The entire sve can be loaded
                    auto const simd_vector_str{
                        ::uwvm2::utils::intrinsics::arm_sve::svld1_u8(all_one_predicate_reg, reinterpret_cast<uint8_t_const_may_alias_ptr>(section_curr))};

                    auto const check_upper{::uwvm2::utils::intrinsics::arm_sve::svcmpge_n_u8(all_one_predicate_reg, simd_vector_str, simd_vector_check)};

                    if(::uwvm2::utils::intrinsics::arm_sve::svptest_any(all_one_predicate_reg, check_upper)) [[unlikely]]
                    {
                        need_change_u8_1b_view_to_vec = true;
                        return;
                    }
                    else
                    {
                        // all are single bytes, so there are 64
                        // There is no need to staging func_counter_this_round_end, as the special case no longer exists.
                        func_counter += svc_sz;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        section_curr += svc_sz;

                        // [before_section ... | func_count ... typeidx1 ... (svc_sz - 1) ...] typeidx64
                        // [                        safe                                     ] unsafe (could be the section_end)
                        //                                                                     ^^ section_curr
                    }
                }
            }

            // [before_section ... | func_count ... typeidx1 ... (svc_sz - 1) ...] typeidx64 ... (?) ... ] (end)
            // [                        safe                                                             ] unsafe (could be the section_end)
            //                                                                     ^^ section_curr
            //
            // Or maybe:
            //
            // [before_section ... | func_count ... typeidx1 ... (svc_sz - 1) ...] (end)
            // [                        safe                                     ] unsafe (could be the section_end)
            //                                                                     ^^ section_curr

            // Generate quantifiers for final tail processing
            ::uwvm2::utils::intrinsics::arm_sve::svbool_t load_predicate;  // No initialization necessary

# if SIZE_MAX > UINT32_MAX
            load_predicate = ::uwvm2::utils::intrinsics::arm_sve::svwhilelt_b8_u64(::std::bit_cast<::std::uint64_t>(section_curr),
                                                                                   ::std::bit_cast<::std::uint64_t>(section_end));
# elif SIZE_MAX > UINT16_MAX
            load_predicate = ::uwvm2::utils::intrinsics::arm_sve::svwhilelt_b8_u32(::std::bit_cast<::std::uint32_t>(section_curr),
                                                                                   ::std::bit_cast<::std::uint32_t>(section_end));
# else
#  error "missing instructions"
# endif

            using uint8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint8_t const*;

            // sve safely loads memory via predicates
            auto const simd_vector_str{
                ::uwvm2::utils::intrinsics::arm_sve::svld1_u8(load_predicate, reinterpret_cast<uint8_t_const_may_alias_ptr>(section_curr))};

            // check: simd_vector_str >= simd_vector_check

            auto const check_upper{::uwvm2::utils::intrinsics::arm_sve::svcmpge_n_u8(load_predicate, simd_vector_str, simd_vector_check)};

            if(::uwvm2::utils::intrinsics::arm_sve::svptest_any(load_predicate, check_upper)) [[unlikely]]
            {
                need_change_u8_1b_view_to_vec = true;
                return;
            }
            else
            {
                using uint8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::std::uint8_t const*;

                auto const last_load_predicate_size{static_cast<::std::size_t>(reinterpret_cast<uint8_t_const_may_alias_ptr>(section_end) -
                                                                               reinterpret_cast<uint8_t_const_may_alias_ptr>(section_curr))};

                // all are single bytes, so there are 64
                // There is no need to staging func_counter_this_round_end, as the special case no longer exists.
                func_counter += last_load_predicate_size;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                section_curr += last_load_predicate_size;

                // [before_section ... | func_count ... typeidx1 ... (last_load_predicate_size - 1) ...] (end)
                // [                        safe                                                       ] unsafe (could be the section_end)
                //                                                                                       ^^ section_curr
            }
        }
        ();

        if(need_change_u8_1b_view_to_vec) [[unlikely]]
        {
            return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                              \
    (defined(__AVX512BW__) && (UWVM_HAS_BUILTIN(__builtin_ia32_ptestmb512) || UWVM_HAS_BUILTIN(__builtin_ia32_cmpb512_mask)))
        /// (Little Endian)
        /// mask: x86_64-avx512vbmi
        /// @todo need check

        using i64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int64_t;
        using u64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x16simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint32_t;
        using c8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = char;
        using u8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

        while(static_cast<::std::size_t>(section_end - section_curr) >= 64uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (63) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x64simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x64simd));

            // It's already a little-endian.

            auto const check_upper{simd_vector_str >= simd_vector_check};

            if(
# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestmb512)  // Only supported by GCC
                // gcc:
                // vptestmb        k0, zmm0, zmm0
                // kortestq        k0, k0
                // sete    al
                __builtin_ia32_ptestmb512(::std::bit_cast<c8x64simd>(check_upper), ::std::bit_cast<c8x64simd>(check_upper), UINT64_MAX)
# elif defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_cmpb512_mask)
                // clang:
                // vptestmd        k0, zmm0, zmm0
                // kortestw        k0, k0
                // setne   al
                // vzeroupper
                //
                // gcc:
                // vpxor   xmm1, xmm1, xmm1
                // vpcmpb  k0, zmm0, zmm1, 4
                // kortestq        k0, k0
                // setne   al
                __builtin_ia32_cmpb512_mask(::std::bit_cast<c8x64simd>(check_upper), c8x64simd{}, 0x04, UINT64_MAX)
# else
#  error "missing instructions"
# endif
                    ) [[unlikely]]
            {
                // There are special cases greater than typeidx, which may be legal redundancy 0 or illegal data, all converted to vec to determine.
                return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
            }
            else
            {
                // all are single bytes, so there are 64
                // There is no need to staging func_counter_this_round_end, as the special case no longer exists.
                func_counter += 64u;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                section_curr += 64uz;

                // [before_section ... | func_count ... typeidx1 ... (63) ...] typeidx64
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                              \
    ((defined(__AVX__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz256)) || (defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xbnz_v)))
        /// (Little Endian), [[gnu::vector_size]]
        /// mask: x86_64-avx, loongarch-ASX

        using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
        using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
        using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
        using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

        while(static_cast<::std::size_t>(section_end - section_curr) >= 32uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x32simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x32simd));

            // It's already a little-endian.

            auto const check_upper{simd_vector_str >= simd_vector_check};

            if(
# if defined(__AVX__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz256)
                !__builtin_ia32_ptestz256(::std::bit_cast<i64x4simd>(check_upper), ::std::bit_cast<i64x4simd>(check_upper))
# elif defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xbnz_v)
                __builtin_lasx_xbnz_v(::std::bit_cast<u8x32simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                    ) [[unlikely]]
            {
                // There are special cases greater than typeidx, which may be legal redundancy 0 or illegal data, all converted to vec to determine.
                return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
            }
            else
            {
                // all are single bytes, so there are 32
                func_counter += 32u;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                section_curr += 32uz;

                // [before_section ... | func_count ... typeidx1 ... (31) ...] typeidx32
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }

#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                           \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) ||                                                                                    \
     (defined(__ARM_NEON) && (UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32) || UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu))) ||                  \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)) ||                                                                                     \
     (defined(__wasm_simd128__) && (UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16) || UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16))))
        /// (Little Endian), [[gnu::vector_size]]
        /// x86_64-sse2, aarch64-neon, loongarch-SX, wasm-wasmsimd128

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

        while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x16simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

            // It's already a little-endian.

            auto const check_upper{simd_vector_str >= simd_vector_check};

            if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                    ) [[unlikely]]
            {
                // There are special cases greater than typeidx, which may be legal redundancy 0 or illegal data, all converted to vec to determine.
                return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
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

                section_curr += 16uz;

                // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }
#endif

#if ((defined(_MSC_VER) && !defined(__clang__)) && !defined(_KERNEL_MODE) && (defined(_M_AMD64) || defined(_M_ARM64))) ||                                      \
    !(defined(__LITTLE_ENDIAN__) && (defined(__ARM_FEATURE_SVE) || (defined(UWVM_ENABLE_SME_SVE_STREAM_MODE) && defined(__ARM_FEATURE_SME))))
        // non-simd or simd (non-sve) tail-treatment
        // msvc, non-sve, default
        // arm-sve no tail treatment required

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // This needs to be judged first before setting the value, to ensure that the special case call view_to_vec can get the correct function_counter
            // Need to stash func_counter_this_round_end because special cases are not yet handled
            auto const func_counter_this_round_end{func_counter + 1u};

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(func_counter_this_round_end > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            // Check for the presence of redundant 0, (check single byte)
            if(static_cast<::std::size_t>(reinterpret_cast<::std::byte const*>(typeidx_next) - section_curr) > 1uz) [[unlikely]]
            {
                return change_u8_1b_view_to_vec(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
            }

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr

            // Processed and returned to stock. Special cases processed.
            func_counter = func_counter_this_round_end;
        }
#endif

        // set view end
        functionsec.funcs.storage.typeidx_u8_view.end = reinterpret_cast<wasm_u8_const_may_alias_ptr>(section_curr);

        return section_curr;
    }

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                                \
    (((defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb256)) && (defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128))) ||            \
     ((defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xvmskltz_b)) && (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b))) ||    \
     ((defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128) && UWVM_HAS_BUILTIN(__builtin_ia32_palignr128)) &&                                     \
      (defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128))) ||                                                                                  \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b) && UWVM_HAS_BUILTIN(__builtin_lsx_vsrli_b) &&                                         \
      UWVM_HAS_BUILTIN(__builtin_lsx_vmskltz_b)))

    // shuffle simd128, need mask_u16 and shuffle

    struct simd128_shuffle_table_t
    {
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;

        u8x16simd shuffle_mask{};   // shuffle mask u8x32 c-> u32x8
        unsigned processed_simd{};  // How many u32 typeidx can be output 0 == error
        unsigned processed_byte{};  // How many bytes are scanned? 0 == error
    };

    template <::std::size_t table_size, ::std::size_t max_leb_size, ::std::uint8_t mask_zero>
        requires (::std::popcount(table_size) == 1u && ::std::popcount(max_leb_size) == 1u)  // Check to see if it is an nth power of 2
    inline constexpr ::fast_io::array<simd128_shuffle_table_t, table_size> generate_simd128_shuffle_table() noexcept
    {
        // Since all shuffles have a channel width of 128, only the 128 version can be done
        constexpr ::std::size_t vector_size{16uz};

        // How many bytes of source data can be read at a time
        constexpr ::std::size_t max_bytes_can_handle_per_round{vector_size / max_leb_size};

        using u8x16simd [[__gnu__::__vector_size__(16)]] = ::std::uint8_t;

        // This value represents the index that is 0 after mask, and you need to make sure that the value after adding 8 can also be masked to 0
        constexpr u8x16simd can_mask_zero{mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero,
                                          mask_zero};

        ::fast_io::array<simd128_shuffle_table_t, table_size> ret{};

        for(::std::size_t i{}; i != table_size; ++i)
        {
            // First set to all mask_zero for subsequent operations
            ret[i].shuffle_mask = can_mask_zero;

            // Storing part of the data for calculations
            ::std::size_t bytes_remaining{max_bytes_can_handle_per_round};
            ::std::size_t temp_i{i};

            ::std::size_t simd_counter{};

            bool error{};

            ::std::size_t j_counter{};

            while(bytes_remaining)
            {
                auto const ctro{static_cast<unsigned>(::std::countr_one(temp_i))};
                if(ctro > max_leb_size - 1u)
                {
                    // The number of bits in the pop exceeds the set number of bits
                    error = true;
                    break;
                }
                else if(ctro == bytes_remaining)
                {
                    // POP at the end makes it impossible to process
                    break;
                }

                auto const shr{ctro + 1u};

                for(unsigned j{}; j != shr; ++j) { ret[i].shuffle_mask[simd_counter * max_leb_size + j] = static_cast<::std::uint8_t>(j_counter++); }

                bytes_remaining -= shr;
                temp_i >>= shr;
                ++simd_counter;
            }

            if(error)
            {
                ret[i].shuffle_mask = can_mask_zero;
                ret[i].processed_simd = 0u;
                ret[i].processed_byte = 0u;
            }
            else
            {
                ret[i].processed_simd = simd_counter;
                ret[i].processed_byte = max_bytes_can_handle_per_round - bytes_remaining;
            }
        }

        return ret;
    }

    // Need to provide the ability to calculate after vector add 8
    inline constexpr ::std::uint8_t mask_zero{static_cast<::std::uint8_t>(0x80u)};
    inline constexpr auto simd128_shuffle_table{generate_simd128_shuffle_table<256uz, 2uz, mask_zero>()};

    /*
     *  simd128_shuffle_table: mask_zero == 80
     *
     *  0b00000000 [00 80 01 80 02 80 03 80 04 80 05 80 06 80 07 80] 8 8
     *  0b00000001 [00 01 02 80 03 80 04 80 05 80 06 80 07 80 80 80] 7 8
     *  0b00000010 [00 80 01 02 03 80 04 80 05 80 06 80 07 80 80 80] 7 8
     *  0b00000011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00000100 [00 80 01 80 02 03 04 80 05 80 06 80 07 80 80 80] 7 8
     *  0b00000101 [00 01 02 03 04 80 05 80 06 80 07 80 80 80 80 80] 6 8
     *  0b00000110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00000111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00001000 [00 80 01 80 02 80 03 04 05 80 06 80 07 80 80 80] 7 8
     *  0b00001001 [00 01 02 80 03 04 05 80 06 80 07 80 80 80 80 80] 6 8
     *  0b00001010 [00 80 01 02 03 04 05 80 06 80 07 80 80 80 80 80] 6 8
     *  0b00001011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00001100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00001101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00001110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00001111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00010000 [00 80 01 80 02 80 03 80 04 05 06 80 07 80 80 80] 7 8
     *  0b00010001 [00 01 02 80 03 80 04 05 06 80 07 80 80 80 80 80] 6 8
     *  0b00010010 [00 80 01 02 03 80 04 05 06 80 07 80 80 80 80 80] 6 8
     *  0b00010011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00010100 [00 80 01 80 02 03 04 05 06 80 07 80 80 80 80 80] 6 8
     *  0b00010101 [00 01 02 03 04 05 06 80 07 80 80 80 80 80 80 80] 5 8
     *  0b00010110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00010111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00011111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00100000 [00 80 01 80 02 80 03 80 04 80 05 06 07 80 80 80] 7 8
     *  0b00100001 [00 01 02 80 03 80 04 80 05 06 07 80 80 80 80 80] 6 8
     *  0b00100010 [00 80 01 02 03 80 04 80 05 06 07 80 80 80 80 80] 6 8
     *  0b00100011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00100100 [00 80 01 80 02 03 04 80 05 06 07 80 80 80 80 80] 6 8
     *  0b00100101 [00 01 02 03 04 80 05 06 07 80 80 80 80 80 80 80] 5 8
     *  0b00100110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00100111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00101000 [00 80 01 80 02 80 03 04 05 06 07 80 80 80 80 80] 6 8
     *  0b00101001 [00 01 02 80 03 04 05 06 07 80 80 80 80 80 80 80] 5 8
     *  0b00101010 [00 80 01 02 03 04 05 06 07 80 80 80 80 80 80 80] 5 8
     *  0b00101011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00101100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00101101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00101110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00101111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00110111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b00111111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01000000 [00 80 01 80 02 80 03 80 04 80 05 80 06 07 80 80] 7 8
     *  0b01000001 [00 01 02 80 03 80 04 80 05 80 06 07 80 80 80 80] 6 8
     *  0b01000010 [00 80 01 02 03 80 04 80 05 80 06 07 80 80 80 80] 6 8
     *  0b01000011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01000100 [00 80 01 80 02 03 04 80 05 80 06 07 80 80 80 80] 6 8
     *  0b01000101 [00 01 02 03 04 80 05 80 06 07 80 80 80 80 80 80] 5 8
     *  0b01000110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01000111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01001000 [00 80 01 80 02 80 03 04 05 80 06 07 80 80 80 80] 6 8
     *  0b01001001 [00 01 02 80 03 04 05 80 06 07 80 80 80 80 80 80] 5 8
     *  0b01001010 [00 80 01 02 03 04 05 80 06 07 80 80 80 80 80 80] 5 8
     *  0b01001011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01001100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01001101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01001110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01001111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01010000 [00 80 01 80 02 80 03 80 04 05 06 07 80 80 80 80] 6 8
     *  0b01010001 [00 01 02 80 03 80 04 05 06 07 80 80 80 80 80 80] 5 8
     *  0b01010010 [00 80 01 02 03 80 04 05 06 07 80 80 80 80 80 80] 5 8
     *  0b01010011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01010100 [00 80 01 80 02 03 04 05 06 07 80 80 80 80 80 80] 5 8
     *  0b01010101 [00 01 02 03 04 05 06 07 80 80 80 80 80 80 80 80] 4 8
     *  0b01010110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01010111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01011111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01100111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01101111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01110111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b01111111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10000000 [00 80 01 80 02 80 03 80 04 80 05 80 06 80 80 80] 7 7
     *  0b10000001 [00 01 02 80 03 80 04 80 05 80 06 80 80 80 80 80] 6 7
     *  0b10000010 [00 80 01 02 03 80 04 80 05 80 06 80 80 80 80 80] 6 7
     *  0b10000011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10000100 [00 80 01 80 02 03 04 80 05 80 06 80 80 80 80 80] 6 7
     *  0b10000101 [00 01 02 03 04 80 05 80 06 80 80 80 80 80 80 80] 5 7
     *  0b10000110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10000111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10001000 [00 80 01 80 02 80 03 04 05 80 06 80 80 80 80 80] 6 7
     *  0b10001001 [00 01 02 80 03 04 05 80 06 80 80 80 80 80 80 80] 5 7
     *  0b10001010 [00 80 01 02 03 04 05 80 06 80 80 80 80 80 80 80] 5 7
     *  0b10001011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10001100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10001101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10001110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10001111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10010000 [00 80 01 80 02 80 03 80 04 05 06 80 80 80 80 80] 6 7
     *  0b10010001 [00 01 02 80 03 80 04 05 06 80 80 80 80 80 80 80] 5 7
     *  0b10010010 [00 80 01 02 03 80 04 05 06 80 80 80 80 80 80 80] 5 7
     *  0b10010011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10010100 [00 80 01 80 02 03 04 05 06 80 80 80 80 80 80 80] 5 7
     *  0b10010101 [00 01 02 03 04 05 06 80 80 80 80 80 80 80 80 80] 4 7
     *  0b10010110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10010111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10011111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10100000 [00 80 01 80 02 80 03 80 04 80 05 06 80 80 80 80] 6 7
     *  0b10100001 [00 01 02 80 03 80 04 80 05 06 80 80 80 80 80 80] 5 7
     *  0b10100010 [00 80 01 02 03 80 04 80 05 06 80 80 80 80 80 80] 5 7
     *  0b10100011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10100100 [00 80 01 80 02 03 04 80 05 06 80 80 80 80 80 80] 5 7
     *  0b10100101 [00 01 02 03 04 80 05 06 80 80 80 80 80 80 80 80] 4 7
     *  0b10100110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10100111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10101000 [00 80 01 80 02 80 03 04 05 06 80 80 80 80 80 80] 5 7
     *  0b10101001 [00 01 02 80 03 04 05 06 80 80 80 80 80 80 80 80] 4 7
     *  0b10101010 [00 80 01 02 03 04 05 06 80 80 80 80 80 80 80 80] 4 7
     *  0b10101011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10101100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10101101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10101110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10101111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10110111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b10111111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11000111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11001111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11010111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11011111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11100111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11101111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11110111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111000 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111001 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111010 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111011 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111100 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111101 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111110 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     *  0b11111111 [80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80] 0 0
     */

#endif

    /// @brief      Maximum typeidx in [2^7, 2^8)
    /// @details    Storing a typeidx takes up 1 bytes, typeidx corresponding uleb128 varies from 1-2 bytes, linear read/write, no simd optimization
    ///             Sequential scanning, correctly handling all cases of uleb128 u32, allowing up to 5 bytes.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_impl_u8_2b(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count >= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 7u &&
                 type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 8u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // Storing a typeidx takes up 1 bytes, use u8_vector
        functionsec.funcs.mode = ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u8_vector;

        // The size comparison between u32 and size_t has already been checked
        functionsec.funcs.storage.typeidx_u8_vector.reserve(static_cast<::std::size_t>(func_count));

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                                \
        ((defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb256)) && (defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128))) ||         \
    ((defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xvmskltz_b)) && (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)))
        /// (Little Endian), [[gnu::vector_size]], has mask-u16, can shuffle, simd128
        /// x86_64-avx3, loongarch-LSX
        // This error handle is provided when simd is unable to handle the error, including when it encounters a memory boundary, or when there is an error that
        // needs to be localized.

        auto error_handler{[&](::std::size_t n) constexpr UWVM_THROWS -> void
                           {
                               // Need to ensure that section_curr to section_curr + n is memory safe
                               // Processing uses section_curr_end to guarantee entry into the loop for inspection.
                               // Also use section_end to ensure that the scanning of the last leb128 is handled correctly, both memory safe.

                               auto const section_curr_end{section_curr + n};

                               // [ ... typeidx1 ... ] (section_curr_end) ... (outer) ]
                               // [     safe     ... ]                    ... (outer) ] unsafe (could be the section_end)
                               //       ^^ section_curr

                               // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead
                               // of '!='

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

                                   ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

                                   functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(
                                       static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

                                   section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                                   // The outer boundary is unknown and needs to be rechecked
                                   // [ ... typeidx1 ...] typeidx2 ...] ...
                                   // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                                   //                     ^^ section_curr
                               }

                               // [before_section ... | func_count ... typeidx1 ... (n - 1) ... ...  ] typeidxN
                               // [                        safe                                      ] unsafe (could be the section_end)
                               //                                                                      ^^ section_curr
                               //                                      [   simd_vector_str  ]  ...   ] (Depends on the size of section_curr in relation to
                               //                                      section_curr_end)
                           }};

        using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
        using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x16simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
        using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x8simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        // No prefetching of table entries is required, ensuring no contamination of the cache

        while(static_cast<::std::size_t>(section_end - section_curr) >= 32uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x32simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x32simd));

            // It's already a little-endian.

            // When the highest bit of each byte is pop, then mask out 1
            auto const check_mask{
# if defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb256)
                static_cast<unsigned>(__builtin_ia32_pmovmskb256(::std::bit_cast<c8x32simd>(simd_vector_str)))
# elif defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xvmskltz_b)
                ::std::bit_cast<::fast_io::array<unsigned, 8uz>>(__builtin_lasx_xvmskltz_b(::std::bit_cast<i8x32simd>(simd_vector_str))).front_unchecked()
# else
#  error "missing instructions"
# endif
            };

            if(
# if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                __builtin_expect_with_probability(static_cast<bool>(check_mask), true, 1.0 / 3.0)
# else
                check_mask
# endif
            )
            {
                ::std::uint16_t const simd_vector_check{static_cast<::std::uint16_t>(type_section_count)};

                auto check_mask_curr{check_mask};  // uleb128 mask

                u8x16simd const first_second_round_simd_u8x16{::std::bit_cast<::fast_io::array<u8x16simd, 2uz>>(simd_vector_str).front_unchecked()};

                // Record the number of bytes processed in the first round
                ::std::uint8_t first_round_handle_bytes{static_cast<::std::uint8_t>(8u)};

                // 1 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // If check_mask_curr is 0, write directly

                    if(!check_table_index)
                    {
                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        //  [... curr ... (7) ... curr_next ... (23) ...]
                        //  [                 safe                      ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(first_second_round_simd_u8x16)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [   needwrite_u8x8x2v0  ]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (7) ... curr ... (23) ...]
                        //  [                  safe                     ] unsafe (could be the section_end)
                        //                             ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... typeidxc ... (23) ...] ...
                        // [                                safe                                          ] unsafe (could be the section_end)
                        //                                                           ^^ section_curr
                        //                                      [                simd_vector_str          ]
                    }
                    else
                    {
                        //  [...] curr ...
                        //  [sf ] unsafe (could be the section_end)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        first_round_handle_bytes = static_cast<::std::uint8_t>(curr_table_processed_byte);

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
                            // [                        safe                             ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                             ^^ section_curr + 16uz

                            // If not yet processed in the first round, it can be processed up to 32
                            error_handler(32uz);
                            // Start the next round straight away
                            continue;
                        }

                        // Remove the second round of processing for the next round of calculations
                        check_mask_curr >>= curr_table_processed_byte;

                        //  [... curr ... (7) ... curr_next ... (23) ...]
                        //  [                 safe                      ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]
                        //      [    ctps  ]  ...]

                        // shuffle (first 16 byte) and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(first_second_round_simd_u8x16, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(
                            __builtin_lsx_vshuf_b(first_second_round_simd_u8x16, first_second_round_simd_u8x16, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
                            // [                        safe                             ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                             ^^ section_curr + 32uz

                            // If not yet processed in the first round, it can be processed up to 32
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(32uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr ... (7) ... curr_next ... (23) ...]
                        //  [                 safe                      ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr ... (7) ... curr_next ... (23) ...]
                        //  [                 safe                      ] unsafe (could be the section_end)
                        //                    ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        // Or: (Security boundaries for writes are checked)
                        //                        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... ... (23) ...] ...
                        // [                        safe                                         ] unsafe (could be the section_end)
                        //                                                   ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [   simd_vector_str              ]
                    }
                }

                ::std::uint8_t second_round_handle_bytes{static_cast<::std::uint8_t>(8u)};

                // 2 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // There are only 7 and 8 and 0 in the table, where the 0 have been handled out of the error,
                    // and if a non-7 or non-8 is encountered, there is a problem with the surface

                    // When first_round_handle_bytes is 7, check_table_index is always greater than 0，
                    // Because the highest bit of the first 8 bits is pop, 0bxxxx'xxxx'1xxx'xxxxu >> 7u == 0bxxxx'xxx1

# if __has_cpp_attribute(assume)
                    [[assume((first_round_handle_bytes == static_cast<::std::uint8_t>(7u) && check_table_index != 0u) ||
                             first_round_handle_bytes == static_cast<::std::uint8_t>(8u))]];
# endif

                    if(!check_table_index)
                    {
                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // Requires a vector right shift to implement

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + first_round_handle_bytes)
                        //                      [      need write       ]..]

                        // When first_round_handle_bytes is 7, check_table_index is always greater than 0，
                        // Because the highest bit of the first 8 bits is pop, 0b...'xxxx'xxxx'1xxx'xxxxu >> 7u == 0b...'xxxx'xxx1

                        UWVM_ASSERT(first_round_handle_bytes == static_cast<::std::uint8_t>(8u));

                        // Here the shuffle simulation element is kept shifted right by 8 bits to facilitate subsequent expansion,
                        // while the subsequent taking of the first address has been shown to be optimized by the llvm into a single instruction

                        u8x16simd simd_vector_str_need_shf{__builtin_shufflevector(first_second_round_simd_u8x16,
                                                                                   first_second_round_simd_u8x16,
                                                                                   8,
                                                                                   9,
                                                                                   10,
                                                                                   11,
                                                                                   12,
                                                                                   13,
                                                                                   14,
                                                                                   15,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1)};

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(simd_vector_str_need_shf)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + first_round_handle_bytes)
                        //                      [      need write       ]..]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (7) ... curr ... (23) ...] ...
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                        ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                       [    needwrite   ]...]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (7) ... curr ... (23) ...] ...
                        //  [                  safe                     ] unsafe (could be the section_end)
                        //                                           ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... typeidxc ... (23) ...] ...
                        // [                                safe                                          ] unsafe (could be the section_end)
                        //                                                                              ^^^^^^ section_curr
                        //                                      [                simd_vector_str          ]
                    }
                    else
                    {
                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing

                        //  [...] curr ... (curr + 8u)
                        //  [sf ] unsafe (could be the section_end)
                        //  [ safe  ... ] unsafe (Left over from above, length unknown, can't be used)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        // Since it's the second round, the data has to be moved back  the number of bytes processed in the first round
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask + first_round_handle_bytes};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        second_round_handle_bytes = static_cast<::std::uint8_t>(curr_table_processed_byte);

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (23) ...] ...
                            // [                        safe                                         ] unsafe (could be the section_end)
                            //                                                   ^^^ ^^^ section_curr (indeterminate location)
                            //                                                                    ^^^ ^^^ section_curr + 8uz

                            // Second round can only handle 32 - first round max 8 = 24
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(24uz);
                            // Start the next round straight away
                            continue;
                        }

                        check_mask_curr >>= curr_table_processed_byte;

                        // shuffle and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(first_second_round_simd_u8x16, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(
                            __builtin_lsx_vshuf_b(first_second_round_simd_u8x16, first_second_round_simd_u8x16, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                            // [                        safe                                        ] unsafe (could be the section_end)
                            //                                                   ^^^^^^^ section_curr (indeterminate location)
                            //                                                                   ^^^^^^^ section_curr + 8uz

                            // Second round can only handle 32 - first round max 8 = 24
                            error_handler(24uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (7) ... curr ... (7) ... ]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                            ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                            [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr_old ... (7) ... curr ... (7) ... ]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                                               ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                            [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                        // [                        safe                                        ] unsafe (could be the section_end)
                        //                                                                   ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [   simd_vector_str             ]
                    }
                }

                auto const first_second_round_handled_bytes{static_cast<unsigned>(first_round_handle_bytes + second_round_handle_bytes)};

                u8x16simd third_fourth_round_simd_u8x16;

                switch(first_second_round_handled_bytes)
                {
                    case 14u:
                    {
                        auto const shuffle_u8x32(__builtin_shufflevector(simd_vector_str,
                                                                         simd_vector_str,
                                                                         14,
                                                                         15,
                                                                         16,
                                                                         17,
                                                                         18,
                                                                         19,
                                                                         20,
                                                                         21,
                                                                         22,
                                                                         23,
                                                                         24,
                                                                         25,
                                                                         26,
                                                                         27,
                                                                         28,
                                                                         29,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1));
                        third_fourth_round_simd_u8x16 = ::std::bit_cast<::fast_io::array<u8x16simd, 2uz>>(shuffle_u8x32).front_unchecked();
                        break;
                    }
                    case 15u:
                    {
                        auto const shuffle_u8x32(__builtin_shufflevector(simd_vector_str,
                                                                         simd_vector_str,
                                                                         15,
                                                                         16,
                                                                         17,
                                                                         18,
                                                                         19,
                                                                         20,
                                                                         21,
                                                                         22,
                                                                         23,
                                                                         24,
                                                                         25,
                                                                         26,
                                                                         27,
                                                                         28,
                                                                         29,
                                                                         30,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1));
                        third_fourth_round_simd_u8x16 = ::std::bit_cast<::fast_io::array<u8x16simd, 2uz>>(shuffle_u8x32).front_unchecked();
                        break;
                    }
                    case 16u:
                    {
                        auto const shuffle_u8x32(__builtin_shufflevector(simd_vector_str,
                                                                         simd_vector_str,
                                                                         16,
                                                                         17,
                                                                         18,
                                                                         19,
                                                                         20,
                                                                         21,
                                                                         22,
                                                                         23,
                                                                         24,
                                                                         25,
                                                                         26,
                                                                         27,
                                                                         28,
                                                                         29,
                                                                         30,
                                                                         31,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1,
                                                                         -1));
                        third_fourth_round_simd_u8x16 = ::std::bit_cast<::fast_io::array<u8x16simd, 2uz>>(shuffle_u8x32).front_unchecked();
                        break;
                    }
                    default:
                    {
                        UWVM_ASSERT(false);
                        ::fast_io::unreachable();
                    }
                }

                // Record the number of bytes processed in the third round
                ::std::uint8_t third_round_handle_bytes{static_cast<::std::uint8_t>(8u)};

                // 3 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // If check_mask_curr is 0, write directly

# if __has_cpp_attribute(assume)
                    [[assume((second_round_handle_bytes == static_cast<::std::uint8_t>(7u) && check_table_index != 0u) ||
                              second_round_handle_bytes == static_cast<::std::uint8_t>(8u))]];
# endif

                    if(!check_table_index)
                    {
                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(third_fourth_round_simd_u8x16)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [   needwrite_u8x8x2v0  ]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr ... (7) ... curr_next ... (>=7) ...]
                        //  [                 safe                       ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (7) ... curr ... (>=7) ...]
                        //  [                  safe                      ] unsafe (could be the section_end)
                        //                             ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... typeidxc ... (>=7) ...] ...
                        // [                                safe                                           ] unsafe (could be the section_end)
                        //                                                           ^^ section_curr
                        //                                      [                simd_vector_str           ]
                    }
                    else
                    {
                        //  [...] curr ...
                        //  [sf ] unsafe (could be the section_end)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        third_round_handle_bytes = static_cast<::std::uint8_t>(curr_table_processed_byte);

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (>=15) ...] ...
                            // [                        safe                               ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                               ^^ section_curr + 16uz

                            // it can be processed up to 32 - 8(max) * 2 == 16
                            error_handler(16uz);
                            // Start the next round straight away
                            continue;
                        }

                        // Remove the second round of processing for the next round of calculations
                        check_mask_curr >>= curr_table_processed_byte;

                        //  [... curr ... (7) ... curr_next ... (>=7) ...]
                        //  [                 safe                       ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]
                        //      [    ctps  ]  ...]

                        // shuffle and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(third_fourth_round_simd_u8x16, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(
                            __builtin_lsx_vshuf_b(third_fourth_round_simd_u8x16, third_fourth_round_simd_u8x16, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                            __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (>=15) ...] ...
                            // [                        safe                               ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                               ^^ section_curr + 16uz

                            // it can be processed up to 32 - 8(max) * 2 == 16
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(16uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr ... (7) ... curr_next ... (>=7) ...]
                        //  [                 safe                       ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr ... (7) ... curr_next ... (>=7) ...]
                        //  [                 safe                       ] unsafe (could be the section_end)
                        //                    ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        // Or: (Security boundaries for writes are checked)
                        //                        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (<=15) ... ... (7) ... ... (7) ...] ...
                        // [                        safe                                                       ] unsafe (could be the section_end)
                        //                                                          ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [                  simd_vector_str             ]
                    }
                }

                // 4 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // There are only 7 and 8 and 0 in the table, where the 0 have been handled out of the error,
                    // and if a non-7 or non-8 is encountered, there is a problem with the surface

                    // When third_round_handle_bytes is 7, check_table_index is always greater than 0，
                    // Because the highest bit of the first 8 bits is pop, 0bxxxx'xxxx'1xxx'xxxxu >> 7u == 0bxxxx'xxx1

# if __has_cpp_attribute(assume)
                    [[assume((third_round_handle_bytes == static_cast<::std::uint8_t>(7u) && check_table_index != 0u) ||
                             third_round_handle_bytes == static_cast<::std::uint8_t>(8u))]];
# endif

                    if(!check_table_index)
                    {
                        // It's the last round, but since it's already established, it's better to calculate it directly.

                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        // last round no necessary to check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // Requires a vector right shift to implement

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + third_round_handle_bytes)
                        //                      [      need write       ]..]

                        // When third_round_handle_bytes is 7, check_table_index is always greater than 0，
                        // Because the highest bit of the first 8 bits is pop, 0bxxxx'xxxx'1xxx'xxxxu >> 7u == 0bxxxx'xxx1

                        UWVM_ASSERT(third_round_handle_bytes == static_cast<::std::uint8_t>(8u));

                        // Here the shuffle simulation element is kept shifted right by 8 bits to facilitate subsequent expansion,
                        // while the subsequent taking of the first address has been shown to be optimized by the llvm into a single instruction

                        u8x16simd simd_vector_str_need_shf{__builtin_shufflevector(third_fourth_round_simd_u8x16,
                                                                                   third_fourth_round_simd_u8x16,
                                                                                   8,
                                                                                   9,
                                                                                   10,
                                                                                   11,
                                                                                   12,
                                                                                   13,
                                                                                   14,
                                                                                   15,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1,
                                                                                   -1)};

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(simd_vector_str_need_shf)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + first_round_handle_bytes)
                        //                      [      need write       ]..]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (<=15) ... curr ... (7) ...] ...
                        //  [                 safe                       ] unsafe (could be the section_end)
                        //                           ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                          [    needwrite   ]...]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (<=15) ... curr ... (7) ...] ...
                        //  [                  safe                       ] unsafe (could be the section_end)
                        //                                              ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (<=23) ... typeidxc ... (7) ...] ...
                        // [                                safe                                            ] unsafe (could be the section_end)
                        //                                                                                ^^^^^^ section_curr
                        //                                      [                simd_vector_str            ]
                    }
                    else
                    {
                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing

                        //  [...] curr ... (curr + 8u)
                        //  [sf ] unsafe (could be the section_end)
                        //  [ safe  ... ] unsafe (Left over from above, length unknown, can't be used)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        // Since it's the second round, the data has to be moved back  the number of bytes processed in the third round
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask + third_round_handle_bytes};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                            // [                        safe                                        ] unsafe (could be the section_end)
                            //                                                   ^^^ ^^^ section_curr (indeterminate location)
                            //                                                                   ^^^ ^^^ section_curr + 8uz

                            // fourth round can only handle 32 - 8 * 3 == 8
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(8uz);
                            // Start the next round straight away
                            continue;
                        }

                        // last round no necessary to check_mask_curr >>= curr_table_processed_byte;

                        // shuffle and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(third_fourth_round_simd_u8x16, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(
                            __builtin_lsx_vshuf_b(third_fourth_round_simd_u8x16, third_fourth_round_simd_u8x16, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                            __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                            // [                        safe                                        ] unsafe (could be the section_end)
                            //                                                   ^^^^^^^ section_curr (indeterminate location)
                            //                                                                   ^^^^^^^ section_curr + 8uz

                            // fourth round can only handle 32 - 8 * 3 == 8
                            error_handler(8uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (<=23) ... curr ... (7) ... ]
                        //  [                 safe                        ] unsafe (could be the section_end)
                        //                               ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                               [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr_old ... (<=23) ... curr ... (7) ... ]
                        //  [                 safe                        ] unsafe (could be the section_end)
                        //                                                  ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                               [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (<=23) ... ... (7) ...] ...
                        // [                        safe                                           ] unsafe (could be the section_end)
                        //                                                                      ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [   simd_vector_str                ]
                    }
                }
            }
            else
            {
                // All correct, can change state: func_counter, section_curr

                // all are single bytes, so there are 32
                func_counter += 32u;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                // Since everything is less than 128, there is no need to check the typeidx.

                // write data
                ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                   ::std::addressof(simd_vector_str),
                                                   sizeof(u8x32simd));

                functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 32uz;

                section_curr += 32uz;

                // [before_section ... | func_count ... typeidx1 ... (31) ...] typeidx32
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                              \
    (((defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128) && UWVM_HAS_BUILTIN(__builtin_ia32_palignr128)) &&                                     \
      (defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128))) ||                                                                                  \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b) && UWVM_HAS_BUILTIN(__builtin_lsx_vsrli_b) &&                                         \
      UWVM_HAS_BUILTIN(__builtin_lsx_vmskltz_b)))

        /// (Little Endian), [[gnu::vector_size]], has mask-u16, can shuffle, simd128
        /// x86_64-ssse3, loongarch-SX

        // This error handle is provided when simd is unable to handle the error, including when it encounters a memory boundary, or when there is an error that
        // needs to be localized.

        auto error_handler{[&](::std::size_t n) constexpr UWVM_THROWS -> void
                           {
                               // Need to ensure that section_curr to section_curr + n is memory safe
                               // Processing uses section_curr_end to guarantee entry into the loop for inspection.
                               // Also use section_end to ensure that the scanning of the last leb128 is handled correctly, both memory safe.

                               auto const section_curr_end{section_curr + n};

                               // [ ... typeidx1 ... ] (section_curr_end) ... (outer) ]
                               // [     safe     ... ]                    ... (outer) ] unsafe (could be the section_end)
                               //       ^^ section_curr

                               // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead
                               // of '!='

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

                                   ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

                                   functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(
                                       static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

                                   section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                                   // The outer boundary is unknown and needs to be rechecked
                                   // [ ... typeidx1 ...] typeidx2 ...] ...
                                   // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                                   //                     ^^ section_curr
                               }

                               // [before_section ... | func_count ... typeidx1 ... (n - 1) ... ...  ] typeidxN
                               // [                        safe                                      ] unsafe (could be the section_end)
                               //                                                                      ^^ section_curr
                               //                                      [   simd_vector_str  ]   ...  ] (Depends on the size of section_curr in relation to
                               //                                      section_curr_end)
                           }};

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x8simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        // No prefetching of table entries is required, ensuring no contamination of the cache

        while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x16simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

            // It's already a little-endian.

            // When the highest bit of each byte is pop, then mask out 1
            auto const check_mask{
# if defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                static_cast<unsigned>(__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(simd_vector_str)))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vmskltz_b)
                ::std::bit_cast<::fast_io::array<unsigned, 4uz>>(__builtin_lsx_vmskltz_b(::std::bit_cast<i8x16simd>(simd_vector_str))).front_unchecked()
# else
#  error "missing instructions"
# endif
            };

            // Since mask 16-bit stuff, it can be assumed that mask is less than 2^16
# if __has_cpp_attribute(assume)
            [[assume(check_mask < 1u << 16u)]];
# endif

            if(
# if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                __builtin_expect_with_probability(static_cast<bool>(check_mask), true, 1.0 / 3.0)
# else
                check_mask
# endif
            )
            {
                ::std::uint16_t const simd_vector_check{static_cast<::std::uint16_t>(type_section_count)};

                auto check_mask_curr{check_mask};  // uleb128 mask

                // Record the number of bytes processed in the first round
                ::std::uint8_t first_round_handle_bytes{static_cast<::std::uint8_t>(8u)};

                // 1 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // If check_mask_curr is 0, write directly

                    if(!check_table_index)
                    {
                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(simd_vector_str)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [   needwrite_u8x8x2v0  ]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr ... (7) ... curr_next ... (7) ...]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (7) ... curr ... (7) ...]
                        //  [                  safe                    ] unsafe (could be the section_end)
                        //                             ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... typeidxc ... (7) ...] ...
                        // [                                safe                                         ] unsafe (could be the section_end)
                        //                                                           ^^ section_curr
                        //                                      [                simd_vector_str         ]
                    }
                    else
                    {
                        //  [...] curr ...
                        //  [sf ] unsafe (could be the section_end)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        first_round_handle_bytes = static_cast<::std::uint8_t>(curr_table_processed_byte);

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
                            // [                        safe                             ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                             ^^ section_curr + 16uz

                            // If not yet processed in the first round, it can be processed up to 16
                            error_handler(16uz);
                            // Start the next round straight away
                            continue;
                        }

                        // Remove the second round of processing for the next round of calculations
                        check_mask_curr >>= curr_table_processed_byte;

                        //  [... curr ... (7) ... curr_next ... (7) ...]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]
                        //      [    ctps  ]  ...]

                        // shuffle and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(simd_vector_str, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_lsx_vshuf_b(simd_vector_str, simd_vector_str, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                            __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
                            // [                        safe                             ] unsafe (could be the section_end)
                            //                                      ^^ section_curr
                            //                                                             ^^ section_curr + 16uz

                            // If not yet processed in the first round, it can be processed up to 16
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(16uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr ... (7) ... curr_next ... (7) ...]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr ... (7) ... curr_next ... (7) ...]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                    ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        // Or: (Security boundaries for writes are checked)
                        //                        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                        // [                        safe                                        ] unsafe (could be the section_end)
                        //                                                   ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [   simd_vector_str             ]
                    }
                }

                // 2 round
                {
                    unsigned const check_table_index{check_mask_curr & 0xFFu};

                    // There are only 7 and 8 and 0 in the table, where the 0 have been handled out of the error,
                    // and if a non-7 or non-8 is encountered, there is a problem with the surface

                    // When first_round_handle_bytes is 7, check_table_index is always greater than 0，
                    // Because the highest bit of the first 8 bits is pop, 0bxxxx'xxxx'1xxx'xxxxu >> 7u == 0bxxxx'xxx1

# if __has_cpp_attribute(assume)
                    [[assume((first_round_handle_bytes == static_cast<::std::uint8_t>(7u) && check_table_index != 0u) ||
                             first_round_handle_bytes == static_cast<::std::uint8_t>(8u))]];
# endif

                    if(!check_table_index)
                    {
                        // It's the last round, but since it's already established, it's better to calculate it directly.

                        // All correct, can change state: func_counter, section_curr

                        // The first 8 bits are single byte typeidx is writable at once.

                        // last round no necessary to check_mask_curr >>= 8u;

                        // check func_counter before write
                        func_counter += 8u;

                        // check counter
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // Since everything is less than 128, there is no need to check the typeidx.

                        // Requires a vector right shift to implement

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + first_round_handle_bytes)
                        //                      [      need write       ]..]

                        // When first_round_handle_bytes is 7, check_table_index is always greater than 0，
                        // Because the highest bit of the first 8 bits is pop, 0bxxxx'xxxx'1xxx'xxxxu >> 7u == 0bxxxx'xxx1

                        UWVM_ASSERT(first_round_handle_bytes == static_cast<::std::uint8_t>(8u));

                        // Here the shuffle simulation element is kept shifted right by 8 bits to facilitate subsequent expansion,
                        // while the subsequent taking of the first address has been shown to be optimized by the llvm into a single instruction

                        u8x16simd simd_vector_str_need_shf{
                            __builtin_shufflevector(simd_vector_str, simd_vector_str, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1)};

                        // write 8 byte
                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;

                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(simd_vector_str_need_shf)};

                        //  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
                        // [       last write      ]
                        //                      ^^^^^^ curr (begin + first_round_handle_bytes)
                        //                      [      need write       ]..]

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (7) ... curr ... (7) ...] ...
                        //  [                 safe                    ] unsafe (could be the section_end)
                        //                        ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                       [    needwrite   ]...]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 8u;

                        //  [... curr_last ... (7) ... curr ... (7) ...] ...
                        //  [                  safe                    ] unsafe (could be the section_end)
                        //                                           ^^^^^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        section_curr += 8u;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... typeidxc ... (7) ...] ...
                        // [                                safe                                         ] unsafe (could be the section_end)
                        //                                                                             ^^^^^^ section_curr
                        //                                      [                simd_vector_str         ]
                    }
                    else
                    {
                        // The write size is 8 bits, but the valid data may be less than 8 bits, directly check the maximum value, more than that, then enter
                        // the tail processing

                        //  [...] curr ... (curr + 8u)
                        //  [sf ] unsafe (could be the section_end)
                        //  [ safe  ... ] unsafe (Left over from above, length unknown, can't be used)
                        //        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr

                        // Cannot change state until error checking: func_counter, section_curr
                        if(func_counter + 8u > func_count) [[unlikely]]
                        {
                            // Near the end, jump directly out of the simd section and hand it over to the tail.
                            break;
                        }

                        //  [... curr ... (7) ...]
                        //  [        safe        ] unsafe (could be the section_end)
                        //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //      [ needwrite ] ...] (Always writes 8 bits, but valid data may be less than 8 bits)

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        // Since it's the second round, the data has to be moved back  the number of bytes processed in the first round
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask + first_round_handle_bytes};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32
                        auto const curr_table_processed_byte{curr_table.processed_byte};  // size of handled uleb128

                        // When the number of consecutive bits is greater than 2, switch back to the normal processing method
                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                            // [                        safe                                        ] unsafe (could be the section_end)
                            //                                                   ^^^ ^^^ section_curr (indeterminate location)
                            //                                                                   ^^^ ^^^ section_curr + 8uz

                            // Second round can only handle 16 - first round max 8 = 8
                            // Ensure that there are no modifications to the state: func_counter, section_curr
                            error_handler(8uz);
                            // Start the next round straight away
                            continue;
                        }

                        // last round no necessary to check_mask_curr >>= curr_table_processed_byte;

                        // shuffle and write

                        u16x8simd mask_res;

# if defined(__SSSE3__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb128)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_ia32_pshufb128(simd_vector_str, curr_table_shuffle_mask));
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_vshuf_b)
                        mask_res = ::std::bit_cast<u16x8simd>(__builtin_lsx_vshuf_b(simd_vector_str, simd_vector_str, curr_table_shuffle_mask));
# else
#  error "missing instructions"
# endif

                        // The leb128 that has been merged into u16x8 is finalized to get the value corresponding to u16

                        auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                        // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                        auto const check_upper{res >= simd_vector_check};

                        if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                            !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                            __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                            __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))  /// @todo need check
# else
#  error "missing instructions"
# endif
                                ) [[unlikely]]
                        {
                            // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                            // [                        safe                                        ] unsafe (could be the section_end)
                            //                                                   ^^^^^^^ section_curr (indeterminate location)
                            //                                                                   ^^^^^^^ section_curr + 8uz

                            // Second round can only handle 16 - first round max 8 = 8
                            error_handler(8uz);
                            // Start the next round straight away
                            continue;
                        }

                        // All correct, can change state: func_counter, section_curr

                        // check func_counter before write
                        func_counter += curr_table_processed_simd;

                        // There is no need to check function_counter, because curr_table_processed_simd is always less than or equal to 8u.

                        // Since typeidx can write to a u8, reduce u16x8 to u8x8

                        // Because the value will be overwritten, use -1 to indicate that any value can be written by the fastest means possible.
                        auto const needwrite_u8x16{::std::bit_cast<u8x16simd>(__builtin_shufflevector(::std::bit_cast<u8x16simd>(res),
                                                                                                      ::std::bit_cast<u8x16simd>(res),
                                                                                                      0,
                                                                                                      2,
                                                                                                      4,
                                                                                                      6,
                                                                                                      8,
                                                                                                      10,
                                                                                                      12,
                                                                                                      14,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1,
                                                                                                      -1))};

                        using u8x8simd [[__gnu__::__vector_size__(8)]] [[maybe_unused]] = ::std::uint8_t;
                        auto const needwrite_u8x8x2{::std::bit_cast<::fast_io::array<u8x8simd, 2uz>>(needwrite_u8x16)};

                        auto const needwrite_u8x8x2v0{needwrite_u8x8x2.front_unchecked()};

                        //  [... curr_old ... (7) ... curr ... (7) ... ]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                            ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                            [    needwrite   ]

                        ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                           ::std::addressof(needwrite_u8x8x2v0),
                                                           sizeof(u8x8simd));

                        functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += curr_table_processed_simd;

                        //  [... curr_old ... (7) ... curr ... (7) ... ]
                        //  [                 safe                     ] unsafe (could be the section_end)
                        //                                               ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                        //                            [    needwrite   ]

                        section_curr += curr_table_processed_byte;

                        // [before_section ... | func_count ... typeidx1 ... (7) ... ... (7) ...] ...
                        // [                        safe                                        ] unsafe (could be the section_end)
                        //                                                                   ^^^^^^^ section_curr (curr_table_processed_byte <= 8)
                        //                                      [   simd_vector_str             ]
                    }
                }
            }
            else
            {
                // All correct, can change state: func_counter, section_curr

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

                // Since everything is less than 128, there is no need to check the typeidx.

                // write data
                ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                   ::std::addressof(simd_vector_str),
                                                   sizeof(u8x16simd));

                functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 16uz;

                section_curr += 16uz;

                // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }
#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                                                           \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) ||                                                                                    \
     (defined(__ARM_NEON) && (UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32) || UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu))) ||                  \
     (defined(__wasm_simd128__) && (UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16) || UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16))))
        /// (Little Endian), [[gnu::vector_size]], no mask-u16, simd128
        ///  x86_64-sse2, aarch64-neon, wasm-simd128 (Since the wasm simd128 shuffle requirement is immediate, it cannot be used.)
        ///  Since the arm neon does not have a move mask, the shuffle algorithm cannot be used.

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
        {
            // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x16simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

            // It's already a little-endian.

            // Using "x > 127u" instead of "x & 0x80u" guarantees that the result is fully bit-set. The rest is left to the compiler to optimize
            auto const check_highest_bit{simd_vector_str > static_cast<::std::uint8_t>(127u)};

            auto const has_pop_highest_bit{
# if defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_highest_bit))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_highest_bit))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_highest_bit))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~check_highest_bit))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(check_highest_bit))
# else
#  error "missing instructions"
# endif
            };

            if(
# if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                __builtin_expect_with_probability(static_cast<bool>(has_pop_highest_bit), true, 1.0 / 3.0)
# else
                has_pop_highest_bit
# endif
            )
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

                    ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

                    functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(
                        static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

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

                // Since everything is less than 128, there is no need to check the typeidx.

                // write data
                ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                   ::std::addressof(simd_vector_str),
                                                   sizeof(u8x16simd));

                functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 16uz;

                section_curr += 16uz;

                // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }
#endif

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

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

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

    /// @brief      Maximum typeidx in [2^8, 2^14)
    /// @details    Storing a typeidx takes up 2 bytes, typeidx corresponding uleb128 varies from 1-2 bytes, linear read/write, no simd optimization
    ///             Sequential scanning, correctly handling all cases of uleb128 u32, allowing up to 5 bytes.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_impl_u16_2b(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count >= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 8u &&
                 type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 14u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // Storing a typeidx takes up 2 bytes, use u16_vector
        functionsec.funcs.mode = ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u16_vector;

        // The size comparison between u32 and size_t has already been checked
        functionsec.funcs.storage.typeidx_u16_vector.reserve(static_cast<::std::size_t>(func_count));

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        /// @todo simd support

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

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            functionsec.funcs.storage.typeidx_u16_vector.emplace_back_unchecked(static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>(typeidx));

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

    /// @brief      Maximum typeidx in [2^14, 2^16)
    /// @details    Storing a typeidx takes up 2 bytes, typeidx corresponding uleb128 varies from 1-3 bytes, linear read/write, no simd optimization
    ///             Sequential scanning, correctly handling all cases of uleb128 u32, allowing up to 5 bytes.
    ///             simd has basically no advantage in this area of processing
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_impl_u16_3b(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count >= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 14u &&
                 type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 16u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // Storing a typeidx takes up 2 bytes, use u16_vector
        functionsec.funcs.mode = ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u16_vector;

        // The size comparison between u32 and size_t has already been checked
        functionsec.funcs.storage.typeidx_u16_vector.reserve(static_cast<::std::size_t>(func_count));

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

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            functionsec.funcs.storage.typeidx_u16_vector.emplace_back_unchecked(static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>(typeidx));

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

    /// @brief      Maximum typeidx in [2^16, 2^32)
    /// @details    Storing a typeidx takes up 4 bytes, typeidx corresponding uleb128 varies from 1-5 bytes, linear read/write, no simd optimization
    ///             Sequential scanning, correctly handling all cases of uleb128 u32, allowing up to 5 bytes.
    ///             simd has basically no advantage in this area of processing
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_impl_u32_5b(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

#if __has_cpp_attribute(assume)
        [[assume(type_section_count >= static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 16u)]];
#endif

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

        // Storing a typeidx takes up 4 bytes, use u32_vector
        functionsec.funcs.mode = ::uwvm2::parser::wasm::standard::wasm1::features::vectypeidx_minimize_storage_mode::u32_vector;

        // The size comparison between u32 and size_t has already been checked
        functionsec.funcs.storage.typeidx_u32_vector.reserve(static_cast<::std::size_t>(func_count));

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

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

            functionsec.funcs.storage.typeidx_u32_vector.emplace_back_unchecked(typeidx);

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

    /// @brief Define the handler function for type_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t> sec_adl,
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
        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t>(module_storage.sections)};

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

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_count;  // No initialization necessary

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

        // The size_t of some platforms is smaller than u32, in these platforms you need to do a size check before conversion
        constexpr auto size_t_max{::std::numeric_limits<::std::size_t>::max()};
        constexpr auto wasm_u32_max{::std::numeric_limits<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>::max()};
        if constexpr(size_t_max < wasm_u32_max)
        {
            // The size_t of current platforms is smaller than u32, in these platforms you need to do a size check before conversion
            if(func_count > size_t_max) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u64 = static_cast<::std::uint_least64_t>(func_count);
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::size_exceeds_the_maximum_value_of_size_t;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }
        }

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(func_count_next);  // never out of bounds

        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // [before_section ... | func_count ...] typeidx1 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        // handle it
        auto const type_section_count{static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(typesec.types.size())};

        if(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 7u)
        {
            // storage: 1 byte
            // vectypeidx: almost always 1 byte, may be redundant zeros (up to 5 byte)
            section_curr = scan_function_section_impl_u8_1b(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }
        else if(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 8u)
        {
            // storage: 1 byte
            // vectypeidx: most 1 byte - 2 byte, may be redundant zeros (up to 5 byte)
            section_curr = scan_function_section_impl_u8_2b(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }
        else if(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 14u)
        {
            // storage: 2 byte
            // vectypeidx: most 1 byte - 2 byte, may be redundant zeros (up to 5 byte)
            section_curr = scan_function_section_impl_u16_2b(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }
        else if(type_section_count < static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>(1u) << 16u)
        {
            // storage: 2 byte
            // vectypeidx uleb128: most: 1 byte - 3 byte, may be redundant zeros (up to 5 byte)
            section_curr = scan_function_section_impl_u16_3b(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }
        else
        {
            // storage: 4 byte
            // vectypeidx uleb128: most: 1 byte - 5 byte
            section_curr = scan_function_section_impl_u32_5b(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
        }

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
    template <>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t>
    {
        inline static constexpr bool value = true;
    };

    template <>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
