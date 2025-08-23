#pragma once

#include "voxlet/containers/string.hpp"

#include <bit>
#include <cassert>
#include <cstring>
#include <memory>
#include <print>
#include <ranges>

#include "voxlet/containers/views/stringSlice.hpp"
#include "voxlet/memory.hpp"


namespace vx::containers {
	constexpr String::String() noexcept {
		if consteval {
			m_long.size = 0uz;
			m_long.capacity = 0uz;
			m_long.data = nullptr;
		}
		else {
			for (const std::size_t i : std::views::iota(0uz, FOOTPRINT))
				m_raw[i] = static_cast<std::byte> (0);
		}
	}

	constexpr String::~String() {
		if (this->isShort())
			return;
		if (m_long.data != nullptr)
			delete[] m_long.data;
		m_long.data = nullptr;
	}

	constexpr String::String(String&& other) noexcept {
		if consteval {
			m_long.size = other.m_long.size;
			m_long.capacity = other.m_long.capacity;
			m_long.data = other.m_long.data;

			other.m_long.size = 0uz;
			other.m_long.capacity = 0uz;
			other.m_long.data = nullptr;
		}
		else {
			for (const std::size_t i : std::views::iota(0uz, FOOTPRINT)) {
				m_raw[i] = other.m_raw[i];
				other.m_raw[i] = static_cast<std::byte> (0);
			}
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
		if (N != 0 && raw[N - 1] == u8'\0')
			--N;
		if (N == 0)
			return string;
		string.reserve(N);
		vx::memory::memcpy(string.getData(), raw, N);
		string.setSize(N);
		return string;
	}

	constexpr auto String::from(const vx::containers::views::StringSlice& slice) noexcept -> String {
		return String::from(std::to_address(slice.begin()), slice.getSize());
	}

	constexpr auto String::from(const vx::containers::views::UncheckedStringSlice& slice) noexcept -> String {
		return String::from(std::to_address(slice.begin()), slice.getSize());
	}


	constexpr auto String::copy() const noexcept -> String {
		return String::from(this->getData(), this->getSize());
	}

	constexpr auto String::slice() const noexcept -> vx::containers::views::StringSlice {
		return vx::containers::views::StringSlice::from(*this);
	}

	constexpr auto String::slice(const size_type start, const size_type end) const
		noexcept
		-> vx::containers::views::StringSlice
	{
		return vx::containers::views::StringSlice::from(*this).slice(
			start,
			end == npos ? vx::containers::views::StringSlice::npos : end
		);
	}

	constexpr auto String::slice(const_iterator start, std::optional<const_iterator> end) const
		noexcept
		-> vx::containers::views::StringSlice
	{
		assert(start.m_container == this);
		assert(!end || end->m_container == this);
		const const_iterator begin {this->begin()};
		return vx::containers::views::StringSlice::from(*this).slice(
			start - begin,
			!!end ? *end - begin : vx::containers::views::StringSlice::npos
		);
	}

	constexpr auto String::unchecked() const noexcept -> UncheckedStringSlice {
		const value_type* const data {this->getData()};
		return vx::containers::views::UncheckedStringSlice{data, data + this->getSize()};
	}


	constexpr auto String::operator[](size_type index) noexcept -> value_type& {
		assert(index < this->getSize());
		return this->getData()[index];
	}

	constexpr auto String::operator[](size_type index) const noexcept -> const value_type& {
		return const_cast<String&> (*this)[index];
	}


	constexpr auto String::reserve(const size_type newCapacity) noexcept -> void {
		if !consteval {
			if (newCapacity <= this->getCapacity())
				return;
			assert(newCapacity > SHORT_CAPACITY);
		}
		value_type* const newData {new value_type[newCapacity]};
		const value_type* const oldData {this->getData()};
		const size_type size {this->getSize()};
		vx::memory::memcpy(newData, oldData, size);
		if (!this->isShort())
			delete[] oldData;
		m_long.capacity = newCapacity;
		m_long.size = size | ~LONG_SIZE_MASK;
		m_long.data = newData;
	}

	constexpr auto String::resize(size_type newSize) noexcept -> void {
		const size_type size {this->getSize()};
		if (newSize <= size)
			return;
		this->reserve(newSize);
		value_type* const data {this->getData()};
		vx::memory::memclear(data + size, newSize - size);
		this->setSize(newSize);
	}


	constexpr auto String::isEmpty() const noexcept -> bool {
		return this->getSize() == 0uz;
	}

	constexpr auto String::getSize() const noexcept -> size_type {
		if (this->isShort())
			return static_cast<size_type> (m_short.size);
		return m_long.size & LONG_SIZE_MASK;
	}

	constexpr auto String::getCapacity() const noexcept -> size_type {
		if (this->isShort())
			return SHORT_CAPACITY;
		return m_long.capacity;
	}


	constexpr auto String::begin() noexcept -> iterator {return iterator{this->getData(), *this};}
	constexpr auto String::end() noexcept -> iterator {return iterator{this->getData() + this->getSize(), *this};}
	constexpr auto String::cbegin() const noexcept -> const_iterator {return const_iterator{this->getData(), *this};}
	constexpr auto String::cend() const noexcept -> const_iterator {
		return const_iterator{this->getData() + this->getSize(), *this};
	}

	constexpr auto String::rbegin() noexcept -> reverse_iterator {return reverse_iterator{this->begin()};}
	constexpr auto String::rend() noexcept -> reverse_iterator {return reverse_iterator{this->end()};}
	constexpr auto String::crbegin() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->cbegin()};
	}
	constexpr auto String::crend() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->cend()};
	}


	constexpr auto String::isPointerValid(const value_type* ptr) const noexcept -> bool {
		const value_type* const data {this->getData()};
		return ptr >= data && ptr < data + this->getSize();
	}

	constexpr auto String::isPointerEnd(const value_type* ptr) const noexcept -> bool {
		return ptr == this->getData() + this->getSize();
	}


	constexpr auto String::isShort() const noexcept -> bool {
		if consteval {
			return false;
		}
		else {
			if constexpr (std::endian::native == std::endian::little)
				return (m_raw[FOOTPRINT - 1uz] & IS_SHORT_MASK) == static_cast<std::byte> (0);
			else
				return (m_raw[0uz] & IS_SHORT_MASK) == static_cast<std::byte> (0);
		}
	}

	constexpr auto String::getData() noexcept -> value_type* {
		if (this->isShort())
			return m_short.data;
		return m_long.data;
	}

	constexpr auto String::getData() const noexcept -> const value_type* {
		return const_cast<String&> (*this).getData();
	}


	constexpr auto String::setSize(size_type size) noexcept -> void {
		if consteval {
			m_long.size = size | ~LONG_SIZE_MASK;
		}
		else {
			if (size <= SHORT_CAPACITY)
				m_short.size = static_cast<short_size_type> (size);
			else
				m_long.size = size | ~LONG_SIZE_MASK;
		}
	}
}
