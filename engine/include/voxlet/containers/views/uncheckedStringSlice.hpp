#pragma once

#include <cstddef>
#include <iterator>
#include <limits>

#include "voxlet/containers/views/uncheckedContiguousIterator.hpp"


namespace vx::containers {
	class String;
}

namespace vx::containers::views {
	class StringSlice;

	class UncheckedStringSlice final {
		friend class vx::containers::String;
		friend class vx::containers::views::StringSlice;

		public:
			using value_type = const char8_t;
			using size_type = std::size_t;
			static constexpr size_type npos = std::numeric_limits<size_type>::max();

			using iterator = vx::containers::views::UncheckedContiguousIterator<value_type, UncheckedStringSlice>;
			using const_iterator = vx::containers::views::UncheckedContiguousIterator<
				const value_type, UncheckedStringSlice
			>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			friend iterator;
			friend const_iterator;

			constexpr UncheckedStringSlice() noexcept = default;
			constexpr ~UncheckedStringSlice() = default;
			constexpr UncheckedStringSlice(const UncheckedStringSlice&) noexcept = default;
			constexpr auto operator=(const UncheckedStringSlice&) noexcept -> UncheckedStringSlice& = default;
			constexpr UncheckedStringSlice(UncheckedStringSlice&&) noexcept = default;
			constexpr auto operator=(UncheckedStringSlice&&) noexcept -> UncheckedStringSlice& = default;

			[[nodiscard]]
			constexpr auto slice(size_type start, size_type end = npos) const noexcept -> UncheckedStringSlice;

			[[nodiscard]]
			constexpr auto operator[](size_type index) const noexcept -> std::remove_const_t<value_type>;

			[[nodiscard]]
			constexpr auto begin() noexcept -> iterator;
			[[nodiscard]]
			constexpr auto end() noexcept -> iterator;
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			[[nodiscard]]
			constexpr auto cbegin() const noexcept -> const_iterator;
			[[nodiscard]]
			constexpr auto cend() const noexcept -> const_iterator;

			[[nodiscard]]
			constexpr auto rbegin() noexcept -> reverse_iterator;
			[[nodiscard]]
			constexpr auto rend() noexcept -> reverse_iterator;
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}
			[[nodiscard]]
			constexpr auto crbegin() const noexcept -> const_reverse_iterator;
			[[nodiscard]]
			constexpr auto crend() const noexcept -> const_reverse_iterator;

			[[nodiscard]]
			constexpr auto isEmpty() const noexcept -> bool;
			[[nodiscard]]
			constexpr auto getSize() const noexcept -> size_type;

			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto empty() const noexcept -> bool {return this->isEmpty();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto size() const noexcept -> size_type {return this->getSize();}


		private:
			constexpr UncheckedStringSlice(value_type* begin, value_type* end) noexcept;

			value_type* m_begin;
			value_type* m_end;
	};
}

#include "voxlet/containers/views/uncheckedStringSlice.inl"

namespace vx {
	using ::vx::containers::views::UncheckedStringSlice;
}
