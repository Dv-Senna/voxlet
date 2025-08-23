#pragma once

#include <cstddef>
#include <limits>


namespace vx::containers {
	class String;
}

namespace vx::containers::views {
	class StringSlice final {
		public:
			using value_type = const char8_t;
			using size_type = std::size_t;
			static constexpr size_type npos = std::numeric_limits<size_type>::max();

			constexpr StringSlice() noexcept = default;
			constexpr ~StringSlice() = default;
			constexpr StringSlice(const StringSlice&) noexcept = default;
			constexpr auto operator=(const StringSlice&) noexcept -> StringSlice& = default;
			constexpr StringSlice(StringSlice&&) noexcept = default;
			constexpr auto operator=(StringSlice&&) noexcept -> StringSlice& = default;

			template <std::size_t N>
			static constexpr auto from(const char8_t (&literal)[N]) noexcept -> StringSlice;
			static constexpr auto from(const char8_t* raw, std::size_t N) noexcept -> StringSlice;
			static constexpr auto from(const vx::containers::String& string) noexcept -> StringSlice;

			constexpr auto slice(size_type start, size_type end = npos) const noexcept -> StringSlice;

			constexpr auto isEmpty() const noexcept -> bool;
			constexpr auto getSize() const noexcept -> size_type;

			[[gnu::always_inline]]
			constexpr auto empty() const noexcept -> bool {return this->isEmpty();}
			[[gnu::always_inline]]
			constexpr auto size() const noexcept -> size_type {return this->getSize();}


		private:
			constexpr StringSlice(value_type* begin, value_type* end) noexcept;

			value_type* m_begin;
			value_type* m_end;
	};
}

#include "voxlet/containers/views/stringSlice.inl"

namespace vx {
	using ::vx::containers::views::StringSlice;
}
