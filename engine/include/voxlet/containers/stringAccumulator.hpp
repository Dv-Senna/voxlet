#pragma once

#include <concepts>
#include <cstddef>
#include <ranges>
#include <type_traits>

#include "voxlet/containers/string.hpp"
#include "voxlet/containers/views/stringSlice.hpp"
#include "voxlet/containers/views/uncheckedStringSlice.hpp"
#include "voxlet/types.hpp"


namespace vx::containers {
	// linked-list like
	template <std::size_t bufferSize = 512uz, bool hasInnerStorage = true>
	class BasicStringAccumulator {
		public:
			BasicStringAccumulator(const BasicStringAccumulator&) = delete;
			auto operator=(const BasicStringAccumulator&) -> BasicStringAccumulator& = delete;

			constexpr BasicStringAccumulator() noexcept = default;
			constexpr ~BasicStringAccumulator();
			constexpr BasicStringAccumulator(BasicStringAccumulator&&) noexcept = default;
			constexpr auto operator=(BasicStringAccumulator&&) noexcept -> BasicStringAccumulator& = default;

			[[nodiscard]]
			constexpr auto getSize() const noexcept -> std::size_t;

			template <std::size_t N>
			constexpr auto push(const char8_t (&literal)[N]) noexcept -> void;
			constexpr auto push(const char8_t* raw, std::size_t size) noexcept -> void;
			constexpr auto push(const vx::String& string) noexcept -> void;
			constexpr auto push(const vx::StringSlice& slice) noexcept -> void;
			constexpr auto push(const vx::containers::views::UncheckedStringSlice& slice) noexcept -> void;
			template <std::ranges::input_range Range>
			requires std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
			constexpr auto push(Range&& range) noexcept -> void;

			template <std::size_t N>
			[[gnu::always_inline]]
			constexpr auto operator+=(const char8_t (&literal)[N]) noexcept -> BasicStringAccumulator& {
				this->push(literal);
				return *this;
			}
			[[gnu::always_inline]]
			constexpr auto operator+=(const vx::String& string) noexcept -> BasicStringAccumulator& {
				this->push(string);
				return *this;
			}
			[[gnu::always_inline]]
			constexpr auto operator+=(const vx::StringSlice& slice) noexcept -> BasicStringAccumulator& {
				this->push(slice);
				return *this;
			}
			[[gnu::always_inline]]
			constexpr auto operator+=(const vx::containers::views::UncheckedStringSlice& slice)
				noexcept
				-> BasicStringAccumulator&
			{
				this->push(slice);
				return *this;
			}
			template <std::ranges::input_range Range>
			requires std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
			[[gnu::always_inline]]
			constexpr auto operator+=(Range&& range) noexcept -> BasicStringAccumulator& {
				this->push(std::forward<Range> (range));
				return *this;
			}

		private:
			struct Segment {
				char8_t data[bufferSize];
				Segment* next;
			};

			[[no_unique_address]]
			std::conditional_t<hasInnerStorage, Segment, vx::types::Empty> m_innerSegment;
			[[no_unique_address]]
			std::conditional_t<!hasInnerStorage, Segment*, vx::types::Empty> m_firstSegment;
			Segment* m_lastSegment;
			std::size_t m_size;
	};

	using StringAccumulator = BasicStringAccumulator<>;
}
