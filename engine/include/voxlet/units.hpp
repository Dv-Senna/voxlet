#pragma once

#include <chrono>


namespace vx {
	using Seconds = std::chrono::duration<float>;
	using Milliseconds = std::chrono::duration<float, std::milli>;
	using Microseconds = std::chrono::duration<float, std::micro>;

	namespace units::time::literals {
		consteval auto operator""_s(long double duration) noexcept {
			return Seconds{static_cast<float> (duration)};
		}
		consteval auto operator""_ms(long double duration) noexcept {
			return Milliseconds{static_cast<float> (duration)};
		}
		consteval auto operator""_µs(long double duration) noexcept {
			return Microseconds{static_cast<float> (duration)};
		}
	}

	namespace units::literals {
		using namespace ::vx::units::time::literals;
	}

	namespace literals {
		using namespace ::vx::units::literals;
	}
}
