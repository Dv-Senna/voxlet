#pragma once

#include "voxlet/containers/views/stringSlice.hpp"

#include "voxlet/containers/string.hpp"
#include <ostream>


namespace vx::containers::views {
	template <std::size_t N>
	constexpr auto StringSlice::from(const char8_t (&literal)[N]) noexcept -> StringSlice {
		return StringSlice::from(literal, N);
	}

	constexpr auto StringSlice::from(const char8_t* const raw, const std::size_t N)
		noexcept
		-> StringSlice
	{
		return StringSlice{raw, raw + N};
	}

	constexpr auto StringSlice::from(const vx::containers::String& string) noexcept -> StringSlice {
		const value_type* const data {string.getData()};
		return StringSlice{data, data + string.getSize()};
	}


	constexpr auto StringSlice::slice(const size_type start, size_type end) const
		noexcept
		-> StringSlice
	{
		const size_type size {this->getSize()};
		assert(start == 0uz || start < size);
		if (end == npos)
			end = size;
		assert(end >= start && end <= size);
		return StringSlice{m_begin + start, m_begin + end};
	}

	constexpr auto StringSlice::unchecked() const noexcept -> UncheckedStringSlice {
		return vx::containers::views::UncheckedStringSlice{m_begin, m_end};
	}


	constexpr auto StringSlice::operator[](size_type index) const noexcept -> std::remove_const_t<value_type> {
		assert(index < this->getSize());
		return m_begin[index];
	}


	constexpr auto StringSlice::begin() noexcept -> iterator {return iterator{m_begin, *this};}
	constexpr auto StringSlice::end() noexcept -> iterator {return iterator{m_end, *this};}
	constexpr auto StringSlice::cbegin() const noexcept -> const_iterator {return const_iterator{m_begin, *this};}
	constexpr auto StringSlice::cend() const noexcept -> const_iterator {return const_iterator{m_end, *this};}

	constexpr auto StringSlice::rbegin() noexcept -> reverse_iterator {return reverse_iterator{this->begin()};}
	constexpr auto StringSlice::rend() noexcept -> reverse_iterator {return reverse_iterator{this->end()};}
	constexpr auto StringSlice::crbegin() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->rbegin()};
	}
	constexpr auto StringSlice::crend() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->rend()};
	}


	constexpr auto StringSlice::isEmpty() const noexcept -> bool {
		return this->getSize() == 0uz;
	}

	constexpr auto StringSlice::getSize() const noexcept -> size_type {
		return static_cast<size_type> (m_end - m_begin);
	}


	constexpr StringSlice::StringSlice(value_type* begin, value_type* end) noexcept :
		m_begin {begin},
		m_end {end}
	{}


	constexpr auto StringSlice::isPointerValid(const value_type* ptr) const noexcept -> bool {
		return ptr >= m_begin && ptr < m_end;
	}

	constexpr auto StringSlice::isPointerEnd(const value_type* ptr) const noexcept -> bool {
		return ptr == m_end;
	}
}
