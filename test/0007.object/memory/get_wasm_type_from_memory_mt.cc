/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      GPT
 * @version     2.0.0
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

// std
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

#ifndef UWVM_MODULE
// import
# include <uwvm2/imported/wasi/wasip1/memory/allocator.h>
#else
# error "Module testing is not currently supported"
#endif

int main()
{
    using namespace ::uwvm2::imported::wasi::wasip1::memory;
    using ::uwvm2::object::memory::linear::allocator_memory_t;

    allocator_memory_t mem{};
    // 2 wasm pages
    mem.init_by_page_count(2u);

    if(mem.memory_begin == nullptr) { return 2; }
    if(mem.memory_length < (1u << 16)) { return 3; }

    constexpr std::size_t static_offset{128u};
    constexpr std::size_t stride{16u};
    constexpr unsigned thread_count{8u};

    // prepare data
    for(unsigned i = 0; i < thread_count; ++i)
    {
        std::size_t dynamic_offset{static_cast<std::size_t>(i) * stride};
        std::size_t off{static_offset + dynamic_offset};
        if(off + sizeof(std::uint32_t) > mem.memory_length) { return 4; }
        auto* p = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::byte*>(mem.memory_begin) + off);
        *p = 0xA5A50000u + i;
    }

    std::atomic<unsigned> mismatch_count{0u};

    // grow thread to interleave with reads
    std::thread grow_thread{[&mem]()
                            {
                                for(int k = 0; k < 4; ++k)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                                    mem.grow(1u);
                                }
                            }};

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for(unsigned i = 0; i < thread_count; ++i)
    {
        threads.emplace_back(
            [&, i]()
            {
                std::uint32_t expected{static_cast<std::uint32_t>(0xA5A50000u + i)};
                std::size_t dynamic_offset{static_cast<std::size_t>(i) * stride};
                for(int round = 0; round < 100000; ++round)
                {
                    auto const v = get_basic_wasm_type_from_memory<std::uint32_t>(mem, static_offset + dynamic_offset);
                    if(v.value != expected)
                    {
                        mismatch_count.fetch_add(1u, std::memory_order_relaxed);
                        break;
                    }
                }
            });
    }

    for(auto& t: threads) { t.join(); }
    grow_thread.join();

    return mismatch_count.load(std::memory_order_relaxed) == 0u ? 0 : 1;
}

