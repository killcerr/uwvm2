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

#ifdef UWVM_MODULE
import fast_io;
#else
// std
# include <cstdint>
# include <cstddef>
# include <cstring>
# include <concepts>
# include <memory>
// import
# include <fast_io.h>
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::utils::container
{

template <typename T>  
struct fast_io_std_allocator 
{  
    using value_type = T;  
    using pointer = T*;  
    using const_pointer = const T*;  
    using reference = T&;  
    using const_reference = const T&;  
    using size_type = ::std::size_t;  
    using difference_type = ::std::ptrdiff_t;  
      
    using fast_io_type_allocator = ::fast_io::native_typed_global_allocator<T>;  
      
    inline constexpr fast_io_std_allocator() noexcept = default;  
      
    template<typename U>  
    inline constexpr fast_io_std_allocator(const fast_io_std_allocator<U>&) noexcept {}  
      
    [[nodiscard]] inline constexpr T* allocate(::std::size_t n) noexcept
    {  
        return fast_io_type_allocator::allocate(n);  
    }  
      
    [[nodiscard]] inline constexpr T* allocate(::std::size_t n, [[maybe_unused]] const void* hint) noexcept
    {  
        return fast_io_type_allocator::allocate(n);  
    }  
      
    [[nodiscard]] inline constexpr ::std::pair<T*, ::std::size_t> allocate_at_least(::std::size_t n) noexcept
    {  
        auto const result{fast_io_type_allocator::allocate_at_least(n)};  
        return {result.ptr, result.count};  
    }  
      
    inline constexpr void deallocate(T* p, ::std::size_t n) noexcept 
    {  
        fast_io_type_allocator::deallocate_n(p, n);  
    }  
      
    inline constexpr ::std::size_t max_size() const noexcept 
    {  
        return ::std::numeric_limits<::std::size_t>::max() / sizeof(T);  
    }  
    
    template<typename U>  
    inline constexpr bool operator==(const fast_io_std_allocator<U>&) const noexcept 
    {  
        return true; 
    }  
      
    template<typename U>  
    inline constexpr bool operator!=(const fast_io_std_allocator<U>&) const noexcept 
    {  
        return false;  
    }  
};  
  

}  // namespace uwvm2::utils::ansies
