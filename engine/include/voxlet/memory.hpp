#pragma once

#include <cstddef>
#include <cstring>
#include <span>
#include <type_traits>
#include <vector>


namespace vx::memory {
	template <typename T>
	requires std::is_trivially_copy_assignable_v<T>
	[[gnu::always_inline]]
	constexpr auto memcpy(T* __restrict dst, const T* __restrict src, const std::size_t size) noexcept -> void {
		if consteval {
			std::ranges::copy(std::span{src, size}, dst);
		}
		else {
			(void)std::memcpy(dst, src, size * sizeof(T));
		}
	}

	template <typename T>
	requires std::is_trivially_copy_assignable_v<T> && std::is_trivially_constructible_v<T>
	[[gnu::always_inline]]
	constexpr auto memmove(T* dst, const T* src, const std::size_t size) noexcept -> void {
		if consteval {
			std::vector<T> buffer {};
			buffer.resize(size);
			std::ranges::copy(std::span{src, size}, buffer.begin());
			std::ranges::copy(buffer, dst);
		}
		else {
			(void)std::memmove(dst, src, size * sizeof(T));
		}
	}

	template <typename T>
	requires std::is_arithmetic_v<T>
	[[gnu::always_inline]]
	constexpr auto memclear(T* mem, const std::size_t size) noexcept -> void {
		if consteval {
			std::ranges::fill(std::span{mem, size}, static_cast<T> (0));
		}
		else {
			(void)std::memset(mem, 0, size * sizeof(T));
		}
	}
}
