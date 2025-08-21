#pragma once

#include "voxlet/containers/string.hpp"

#include <bit>
#include <cstring>
#include <memory>
#include <print>
#include <ranges>


namespace vx::containers {
	constexpr String::String() noexcept {
		for (const std::size_t i : std::views::iota(0uz, FOOTPRINT))
			m_raw[i] = static_cast<std::byte> (0);
	}

	constexpr String::~String() {
		if (this->isShort())
			return;
		if (m_long.data != nullptr)
			delete[] m_long.data;
		m_long.data = nullptr;
	}

	constexpr String::String(String&& other) noexcept {
		for (const std::size_t i : std::views::iota(0uz, FOOTPRINT)) {
			m_raw[i] = other.m_raw[i];
			other.m_raw[i] = static_cast<std::byte> (0);
		}
	}

	constexpr auto String::operator=(String&& other) noexcept -> String& {
		this->~String();
		std::construct_at(this, std::move(other));
		return *this;
	}


	template <String::size_type N>
	constexpr auto String::from(const char8_t (&literal)[N]) noexcept -> String {
		return String::from(literal, N);
	}

	constexpr auto String::from(const char8_t* const raw, size_type N) noexcept -> String {
		String string {};
		if (raw[N - 1] == u8'\0')
			--N;
		if (N <= SHORT_CAPACITY) {
			string.m_short.size = static_cast<std::uint8_t> (N);
			for (const auto i : std::views::iota(0uz, N))
				string.m_short.data[i] = raw[i];
			return string;
		}

		string.m_long.size = (~LONG_SIZE_MASK) | N;
		string.m_long.capacity = N;
		string.m_long.data = new value_type[N];
		for (const auto i : std::views::iota(0uz, N))
			string.m_long.data[i] = raw[i];
		return string;
	}


	constexpr auto String::empty() const noexcept -> bool {
		return this->size() == 0uz;
	}

	constexpr auto String::size() const noexcept -> size_type {
		if (this->isShort())
			return static_cast<size_type> (m_short.size);
		return m_long.size & LONG_SIZE_MASK;
	}

	constexpr auto String::capacity() const noexcept -> size_type {
		if (this->isShort())
			return SHORT_CAPACITY;
		return m_long.capacity;
	}

	constexpr auto String::isShort() const noexcept -> bool {
		if constexpr (std::endian::native == std::endian::little)
			return (m_raw[FOOTPRINT - 1uz] & IS_SHORT_MASK) == static_cast<std::byte> (0);
		else
			return (m_raw[0uz] & IS_SHORT_MASK) == static_cast<std::byte> (0);
	}


	constexpr auto String::getData() noexcept -> value_type* {
		if (this->isShort())
			return m_short.data;
		return m_long.data;
	}

	constexpr auto String::getData() const noexcept -> const value_type* {
		return const_cast<String&> (*this).getData();
	}
}
