#pragma once

#include <cstddef>
#include <iterator>
#include <limits>
#include <ranges>

#include "voxlet/containers/views/checkedContiguousIterator.hpp"
#include "voxlet/containers/views/uncheckedStringSlice.hpp"


namespace vx::containers {
	class String;
}

namespace vx::containers::views {
	class StringSlice final {
		public:
			using value_type = const char8_t;
			using size_type = std::size_t;
			static constexpr size_type npos = std::numeric_limits<size_type>::max();

			using iterator = vx::containers::views::CheckedContiguousIterator<value_type, StringSlice>;
			using const_iterator = vx::containers::views::CheckedContiguousIterator<const value_type, StringSlice>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			friend iterator;
			friend const_iterator;

			constexpr StringSlice() noexcept = default;
			constexpr ~StringSlice() = default;
			constexpr StringSlice(const StringSlice&) noexcept = default;
			constexpr auto operator=(const StringSlice&) noexcept -> StringSlice& = default;
			constexpr StringSlice(StringSlice&&) noexcept = default;
			constexpr auto operator=(StringSlice&&) noexcept -> StringSlice& = default;

			template <std::size_t N>
			[[nodiscard]]
			static constexpr auto from(const char8_t (&literal)[N]) noexcept -> StringSlice;
			[[nodiscard]]
			static constexpr auto from(const char8_t* raw, std::size_t N) noexcept -> StringSlice;
			[[nodiscard]]
			static constexpr auto from(const vx::containers::String& string) noexcept -> StringSlice;

			[[nodiscard]]
			constexpr auto slice(size_type start, size_type end = npos) const noexcept -> StringSlice;
			[[nodiscard]]
			constexpr auto unchecked() const noexcept -> UncheckedStringSlice;

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
			constexpr StringSlice(value_type* begin, value_type* end) noexcept;

			[[nodiscard]]
			constexpr auto isPointerValid(const value_type* ptr) const noexcept -> bool;
			[[nodiscard]]
			constexpr auto isPointerEnd(const value_type* ptr) const noexcept -> bool;

			value_type* m_begin;
			value_type* m_end;
	};

	static_assert(std::ranges::contiguous_range<StringSlice>);
}

#include "voxlet/containers/views/stringSlice.inl"

namespace vx {
	using ::vx::containers::views::StringSlice;
}
