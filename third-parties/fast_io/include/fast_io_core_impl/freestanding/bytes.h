﻿#pragma once

namespace fast_io::details
{

template <typename T>
struct overlapped_copy_buffer_ptr
{
	T *ptr;
	inline explicit constexpr overlapped_copy_buffer_ptr(::std::size_t n) noexcept
		: ptr(::new T[n])
	{}
	inline overlapped_copy_buffer_ptr(overlapped_copy_buffer_ptr const &) = delete;
	inline overlapped_copy_buffer_ptr &operator=(overlapped_copy_buffer_ptr const &) = delete;
	inline constexpr ~overlapped_copy_buffer_ptr()
	{
		::delete[] ptr;
	}
};

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte *bytes_copy_naive_n_impl(::std::byte const *first, ::std::size_t n,
													  ::std::byte *dest) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		::fast_io::details::overlapped_copy_buffer_ptr<::std::byte> tempbuffer(n);
		auto tempbufferptr{tempbuffer.ptr};
		for (::std::size_t i{}; i != n; ++i)
		{
			tempbufferptr[i] = first[i];
		}
		for (::std::size_t i{}; i != n; ++i)
		{
			dest[i] = tempbufferptr[i];
		}
		return dest + n;
	}
	else
	{
		auto ed{first + n};
		auto dested{dest + n};
		if (first <= dest && dest < ed)
		{
			auto firstit{ed};
			auto destit{dested};
			while (firstit != first)
			{
				*(--destit) = *(--firstit);
			}
		}
		else
		{
			auto firstit{first};
			auto destit{dest};
			while (firstit != ed)
			{
				*destit = *firstit;
				++firstit;
				++destit;
			}
		}
		return dested;
	}
}

} // namespace fast_io::details

namespace fast_io::freestanding
{
#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline constexpr ::std::byte *bytes_copy_n(::std::byte const *first, ::std::size_t n, ::std::byte *dest) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		return ::fast_io::details::bytes_copy_naive_n_impl(first, n, dest);
	}
	else
	{
		if (n)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
#if FAST_IO_HAS_BUILTIN(__builtin_memmove)
            __builtin_memmove(dest, first, n);
#else
			::std::memmove(dest, first, n);
#endif
		}
		return dest + n;
	}
}

#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline constexpr ::std::byte *bytes_copy(::std::byte const *first, ::std::byte const *last, ::std::byte *dest) noexcept
{
	return ::fast_io::freestanding::bytes_copy_n(first, static_cast<::std::size_t>(last - first), dest);
}

inline constexpr ::std::byte *nonoverlapped_bytes_copy_n(::std::byte const *first, ::std::size_t n,
														 ::std::byte *dest) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		return ::fast_io::details::bytes_copy_naive_n_impl(first, n, dest);
	}
	else
	{
		if (n)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
            __builtin_memcpy(dest, first, n);
#else
			::std::memcpy(dest, first, n);
#endif
		}
	}
	return dest + n;
}

inline constexpr ::std::byte *nonoverlapped_bytes_copy(::std::byte const *first, ::std::byte const *last,
													   ::std::byte *__restrict dest) noexcept
{
	return nonoverlapped_bytes_copy_n(first, static_cast<::std::size_t>(last - first), dest);
}

template <typename T>
inline constexpr ::std::byte const *type_punning_from_bytes(::std::byte const *__restrict first,
															T &__restrict t) noexcept
{
	constexpr ::std::size_t n{sizeof(T)};
	if constexpr (n != 0)
	{
#if __cpp_lib_bit_cast >= 201806L
		if (__builtin_is_constant_evaluated())
		{
			::std::byte buffer[n];
			nonoverlapped_bytes_copy_n(first, n, buffer);
			t = ::std::bit_cast<T>(buffer);
		}
		else
#endif
		{
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
            __builtin_memcpy(__builtin_addressof(t), first, n);
#else
			::std::memcpy(__builtin_addressof(t), first, n);
#endif
		}
	}
	return first + n;
}

template <::std::size_t n, typename T>
	requires(n <= sizeof(T))
inline constexpr ::std::byte *type_punning_to_bytes_n(T const &__restrict first, ::std::byte *__restrict dest) noexcept
{
	if constexpr (n != 0)
	{
#if __cpp_lib_bit_cast >= 201806L
		if (__builtin_is_constant_evaluated())
		{
			auto buffer{::std::bit_cast<::fast_io::freestanding::array<::std::byte, sizeof(T)>>(first)};
			nonoverlapped_bytes_copy_n(buffer.data(), n, dest);
		}
		else
#endif
		{
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
            __builtin_memcpy(dest, __builtin_addressof(first), n);
#else
			::std::memcpy(dest, __builtin_addressof(first), n);
#endif
		}
	}
	return dest + n;
}

template <typename T>
inline constexpr ::std::byte *type_punning_to_bytes(T const &__restrict first, ::std::byte *__restrict dest) noexcept
{
	constexpr ::std::size_t n{sizeof(T)};
	return type_punning_to_bytes_n<n>(first, dest);
}

inline constexpr ::std::byte *bytes_clear_n(::std::byte *data, ::std::size_t size) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		for (::std::size_t i{}; i != size; ++i)
		{
			data[i] = ::std::byte{};
		}
	}
	else
	{
#if FAST_IO_HAS_BUILTIN(__builtin_memset)
        __builtin_memset(data, 0, size);
#else
		::std::memset(data, 0, size);
#endif
	}
	return data + size;
}

inline constexpr ::std::byte *bytes_clear(::std::byte *first, ::std::byte *last) noexcept
{
	return bytes_clear_n(first, static_cast<::std::size_t>(last - first));
}

inline constexpr ::std::byte *bytes_fill_n(::std::byte *data, ::std::size_t size, ::std::byte val) noexcept
{
	if (__builtin_is_constant_evaluated())
	{
		for (::std::size_t i{}; i != size; ++i)
		{
			data[i] = val;
		}
	}
	else
	{
#if FAST_IO_HAS_BUILTIN(__builtin_memset)
         __builtin_memset(data, static_cast<char unsigned>(val), size);
#else
		::std::memset(data, static_cast<char unsigned>(val), size);
#endif
	}
	return data + size;
}

inline constexpr ::std::byte *bytes_fill(::std::byte *first, ::std::byte *last, ::std::byte val) noexcept
{
	return bytes_fill_n(first, static_cast<::std::size_t>(last - first), val);
}

} // namespace fast_io::freestanding
