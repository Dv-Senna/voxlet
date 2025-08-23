#pragma once

#include "voxlet/containers/views/stringSlice.hpp"

#include "voxlet/containers/string.hpp"


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
		assert(start < size);
		if (end == npos)
			end = size;
		assert(end >= start && end <= size);
		return StringSlice{m_begin + start, m_begin + end};
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
}
