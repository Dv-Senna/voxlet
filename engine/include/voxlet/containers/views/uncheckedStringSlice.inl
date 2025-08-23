#pragma once

#include "voxlet/containers/views/uncheckedStringSlice.hpp"


namespace vx::containers::views {
	constexpr auto UncheckedStringSlice::slice(const size_type start, size_type end) const
		noexcept
		-> UncheckedStringSlice
	{
		const size_type size {this->getSize()};
		if (end == npos)
			end = size;
		return UncheckedStringSlice{m_begin + start, m_begin + end};
	}


	constexpr auto UncheckedStringSlice::operator[](size_type index) const noexcept -> std::remove_const_t<value_type> {
		return m_begin[index];
	}


	constexpr auto UncheckedStringSlice::begin() noexcept -> iterator {return iterator{m_begin, *this};}
	constexpr auto UncheckedStringSlice::end() noexcept -> iterator {return iterator{m_end, *this};}
	constexpr auto UncheckedStringSlice::cbegin() const noexcept -> const_iterator {return const_iterator{m_begin, *this};}
	constexpr auto UncheckedStringSlice::cend() const noexcept -> const_iterator {return const_iterator{m_end, *this};}

	constexpr auto UncheckedStringSlice::rbegin() noexcept -> reverse_iterator {return reverse_iterator{this->begin()};}
	constexpr auto UncheckedStringSlice::rend() noexcept -> reverse_iterator {return reverse_iterator{this->end()};}
	constexpr auto UncheckedStringSlice::crbegin() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->rbegin()};
	}
	constexpr auto UncheckedStringSlice::crend() const noexcept -> const_reverse_iterator {
		return const_reverse_iterator{this->rend()};
	}


	constexpr auto UncheckedStringSlice::isEmpty() const noexcept -> bool {
		return this->getSize() == 0uz;
	}

	constexpr auto UncheckedStringSlice::getSize() const noexcept -> size_type {
		return static_cast<size_type> (m_end - m_begin);
	}


	constexpr UncheckedStringSlice::UncheckedStringSlice(value_type* begin, value_type* end) noexcept :
		m_begin {begin},
		m_end {end}
	{}
}
