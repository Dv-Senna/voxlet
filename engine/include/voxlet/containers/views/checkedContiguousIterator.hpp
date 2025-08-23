#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>


namespace vx::containers::views {
	template <typename ValueType, typename Container>
	class CheckedContiguousIterator final {
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

			constexpr CheckedContiguousIterator() noexcept = default;
			constexpr CheckedContiguousIterator(const CheckedContiguousIterator&) noexcept = default;
			constexpr auto operator=(const CheckedContiguousIterator&) noexcept -> CheckedContiguousIterator& = default;
			constexpr CheckedContiguousIterator(CheckedContiguousIterator&&) noexcept = default;
			constexpr auto operator=(CheckedContiguousIterator&&) noexcept -> CheckedContiguousIterator& = default;

			constexpr auto operator==(const CheckedContiguousIterator& other) const noexcept -> bool {
				assert(other.m_container == m_container);
				return m_ptr == other.m_ptr;
			}
			constexpr auto operator<=> (const CheckedContiguousIterator& other) const noexcept {
				assert(other.m_container == m_container);
				return m_ptr <=> other.m_ptr;
			}

			constexpr auto operator*() const noexcept -> reference {
				assert(m_container->isPointerValid(m_ptr));
				return *m_ptr;
			}
			constexpr auto operator->() const noexcept -> pointer {
				assert(m_container->isPointerValid(m_ptr));
				return m_ptr;
			}
			constexpr auto operator[](difference_type offset) const noexcept -> reference {
				const pointer ptr {m_ptr + offset};
				assert(m_container->isPointerValid(ptr));
				return *ptr;
			}

			constexpr auto operator++() noexcept -> CheckedContiguousIterator& {
				++m_ptr;
				assert(m_container->isPointerValid(m_ptr) || m_container->isPointerEnd(m_ptr));
				return *this;
			}
			constexpr auto operator--() noexcept -> CheckedContiguousIterator& {
				--m_ptr;
				assert(m_container->isPointerValid(m_ptr) || m_container->isPointerEnd(m_ptr));
				return *this;
			}

			constexpr auto operator++(int) noexcept -> CheckedContiguousIterator {
				auto tmp {*this};
				return ++tmp;
			}
			constexpr auto operator--(int) noexcept -> CheckedContiguousIterator {
				auto tmp {*this};
				return --tmp;
			}

			constexpr auto operator+=(difference_type offset) noexcept -> CheckedContiguousIterator& {
				m_ptr += offset;
				assert(m_container->isPointerValid(m_ptr) || m_container->isPointerEnd(m_ptr));
				return *this;
			}
			constexpr auto operator-=(difference_type offset) noexcept -> CheckedContiguousIterator& {
				m_ptr -= offset;
				assert(m_container->isPointerValid(m_ptr) || m_container->isPointerEnd(m_ptr));
				return *this;
			}

			constexpr auto operator+(difference_type offset) const noexcept -> CheckedContiguousIterator {
				auto tmp {*this};
				return  tmp += offset;
			}
			constexpr auto operator-(difference_type offset) const noexcept -> CheckedContiguousIterator {
				auto tmp {*this};
				return  tmp -= offset;
			}
			friend constexpr auto operator+(difference_type offset, CheckedContiguousIterator it)
				noexcept
				-> CheckedContiguousIterator
			{
				return it += offset;
			}

			constexpr auto operator-(const CheckedContiguousIterator& other) const noexcept {
				assert(m_container == other.m_container);
				assert(m_container->isPointerValid(m_ptr) || m_container->isPointerEnd(m_ptr));
				assert(m_container->isPointerValid(other.m_ptr) || m_container->isPointerEnd(m_ptr));
				return m_ptr - other.m_ptr;
			}

			constexpr operator CheckedContiguousIterator<std::add_const_t<value_type>, Container> () const noexcept {
				return CheckedContiguousIterator<std::add_const_t<value_type>, Container> {m_ptr, m_container};
			}

		private:
			constexpr CheckedContiguousIterator(pointer ptr, const Container& container) noexcept :
				m_ptr {ptr},
				m_container {&container}
			{}

			pointer m_ptr;
			const Container* m_container;
	};
}
