#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>


namespace vx::containers::views {
	template <typename ValueType, typename Container>
	class UncheckedContiguousIterator final {
		friend Container;

		public:
			using value_type = ValueType;
			using reference = std::add_lvalue_reference_t<value_type>;
			using const_reference = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
			using pointer = std::add_pointer_t<value_type>;
			using const_pointer = std::add_pointer_t<std::add_const_t<value_type>>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::contiguous_iterator_tag;

			constexpr UncheckedContiguousIterator() noexcept = default;
			constexpr UncheckedContiguousIterator(const UncheckedContiguousIterator&) noexcept = default;
			constexpr auto operator=(const UncheckedContiguousIterator&)
				noexcept
				-> UncheckedContiguousIterator& = default;
			constexpr UncheckedContiguousIterator(UncheckedContiguousIterator&&) noexcept = default;
			constexpr auto operator=(UncheckedContiguousIterator&&) noexcept -> UncheckedContiguousIterator& = default;

			constexpr auto operator==(const UncheckedContiguousIterator& other) const noexcept -> bool {
				return m_ptr == other.m_ptr;
			}
			constexpr auto operator<=> (const UncheckedContiguousIterator& other) const noexcept {
				return m_ptr <=> other.m_ptr;
			}

			constexpr auto operator*() const noexcept -> reference {
				return *m_ptr;
			}
			constexpr auto operator->() const noexcept -> pointer {
				return m_ptr;
			}
			constexpr auto operator[](difference_type offset) const noexcept -> reference {
				const pointer ptr {m_ptr + offset};
				return *ptr;
			}

			constexpr auto operator++() noexcept -> UncheckedContiguousIterator& {
				++m_ptr;
				return *this;
			}
			constexpr auto operator--() noexcept -> UncheckedContiguousIterator& {
				--m_ptr;
				return *this;
			}

			constexpr auto operator++(int) noexcept -> UncheckedContiguousIterator {
				auto tmp {*this};
				return ++tmp;
			}
			constexpr auto operator--(int) noexcept -> UncheckedContiguousIterator {
				auto tmp {*this};
				return --tmp;
			}

			constexpr auto operator+=(difference_type offset) noexcept -> UncheckedContiguousIterator& {
				m_ptr += offset;
				return *this;
			}
			constexpr auto operator-=(difference_type offset) noexcept -> UncheckedContiguousIterator& {
				m_ptr -= offset;
				return *this;
			}

			constexpr auto operator+(difference_type offset) const noexcept -> UncheckedContiguousIterator {
				auto tmp {*this};
				return  tmp += offset;
			}
			constexpr auto operator-(difference_type offset) const noexcept -> UncheckedContiguousIterator {
				auto tmp {*this};
				return  tmp -= offset;
			}
			friend constexpr auto operator+(difference_type offset, UncheckedContiguousIterator it)
				noexcept
				-> UncheckedContiguousIterator
			{
				return it += offset;
			}

			constexpr auto operator-(const UncheckedContiguousIterator& other) const noexcept {
				return m_ptr - other.m_ptr;
			}

			constexpr operator UncheckedContiguousIterator<std::add_const_t<value_type>, Container> () const noexcept {
				return UncheckedContiguousIterator<std::add_const_t<value_type>, Container> {m_ptr, m_container};
			}

		private:
			constexpr UncheckedContiguousIterator(pointer ptr, const Container& container) noexcept :
				m_ptr {ptr},
				m_container {&container}
			{}

			pointer m_ptr;
			const Container* m_container;
	};
}
