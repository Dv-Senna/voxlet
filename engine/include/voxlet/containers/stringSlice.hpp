#pragma once

#include <cstddef>


namespace vx::containers {
	class String;
}

namespace vx::containers::views {
	class StringSlice {
		public:
			using value_type = const char8_t;
			using size_type = std::size_t;

			constexpr StringSlice() noexcept = default;
			constexpr StringSlice(const StringSlice&) noexcept = default;
			constexpr auto operator=(const StringSlice&) noexcept -> StringSlice& = default;
			constexpr StringSlice(StringSlice&&) noexcept = default;
			constexpr auto operator=(StringSlice&&) noexcept -> StringSlice& = default;

		private:
			value_type* m_begin;
			value_type* m_end;
	};
}

namespace vx {
	using ::vx::containers::views::StringSlice;
}
