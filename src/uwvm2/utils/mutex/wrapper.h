/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2.0 License (see LICENSE file).    *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-06-30
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
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <climits>
# include <concepts>
# include <memory>
# include <utility>
# include <type_traits>
# include <atomic>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::mutex
{
    /// @brief Native mutex
    using mutex_t = ::fast_io::native_mutex;
    using mutex_guard_t = ::fast_io::operations::decay::stream_ref_decay_lock_guard<mutex_t&>;
    using unlock_mutex_guard_t = ::fast_io::operations::decay::unlock_stream_ref_decay_lock_guard<mutex_t&>;

    /// @brief Write-Priority RW Lock (Turnstile + RoomEmpty + Atomic Readers Count)
    struct rwlock_t
    {
        mutex_t turnstile{};             // Writers retain access upon entry, preventing new readers from crossing through (write-first priority).
        mutex_t room_empty{};            // Room lock: The writer requires exclusive access; the first reader locks it, and the last reader unlocks it.
        ::std::atomic_size_t readers{};  // Atomic reader count
    };

    /// @brief Shared guard for read operations
    struct rw_shared_guard_t
    {
        rwlock_t* lock_ptr{};

        inline explicit rw_shared_guard_t(rwlock_t& lock) : lock_ptr(std::addressof(lock))
        {
            // Write-first priority: readers first pass through the turnstile, and if there are waiting/ongoing writers, they are blocked here.
            mutex_guard_t t{this->lock_ptr->turnstile};
            // Atomic increments require obtaining room_empty only for transitions from 0 to 1.
            if(this->lock_ptr->readers.fetch_add(1uz, ::std::memory_order_acquire) == 0uz) { this->lock_ptr->room_empty.lock(); }
            // Release the turnstile to allow other readers to enter concurrently.
        }

        inline ~rw_shared_guard_t()
        {
            if(!this->lock_ptr) [[unlikely]] { return; }
            // Atoms decrease, and only transitions from 1 to 0 require releasing room_empty.
            if(this->lock_ptr->readers.fetch_sub(1uz, ::std::memory_order_release) == 1uz) { this->lock_ptr->room_empty.unlock(); }
        }

        rw_shared_guard_t(rw_shared_guard_t const&) = delete;
        rw_shared_guard_t& operator= (rw_shared_guard_t const&) = delete;
    };

    /// @brief Unique guard for write operations
    struct rw_unique_guard_t
    {
        rwlock_t* lock_ptr{};

        inline explicit rw_unique_guard_t(rwlock_t& lock) : lock_ptr(std::addressof(lock))
        {
            // The writer first occupies the turnstile, blocking new readers; then exclusively holds room_empty.
            this->lock_ptr->turnstile.lock();
            this->lock_ptr->room_empty.lock();
        }

        inline ~rw_unique_guard_t()
        {
            if(!this->lock_ptr) [[unlikely]] { return; }
            this->lock_ptr->room_empty.unlock();
            this->lock_ptr->turnstile.unlock();
        }

        rw_unique_guard_t(rw_unique_guard_t const&) = delete;
        rw_unique_guard_t& operator= (rw_unique_guard_t const&) = delete;
    };
}
