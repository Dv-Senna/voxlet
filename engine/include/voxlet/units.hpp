#pragma once

#include <chrono>

#include <flex/typeTraits.hpp>


namespace vx {
	template <typename T>
	struct is_ratio : std::false_type {};

	template <std::intmax_t Num, std::intmax_t Den>
	struct is_ratio<std::ratio<Num, Den>> : std::true_type {};

	template <typename T>
	constexpr auto is_ratio_v = is_ratio<T>::value;

	template <typename T>
	concept ratio = is_ratio_v<T> && requires {
		{T::num} -> std::convertible_to<std::intmax_t>;
		{T::den} -> std::convertible_to<std::intmax_t>;
	};


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


	template <std::unsigned_integral Rep, ratio Ratio>
	class MemorySize {
		using This = MemorySize<Rep, Ratio>;
		template <std::unsigned_integral, ratio>
		friend class MemorySize;

		public:
			constexpr MemorySize() noexcept = default;
			constexpr MemorySize(const This&) noexcept = default;
			constexpr auto operator=(const This&) noexcept -> This& = default;
			constexpr MemorySize(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;

			constexpr auto operator==(const This&) const noexcept -> bool = default;
			constexpr auto operator<=>(const This&) const noexcept = default;

			template <flex::arithmetic T>
			explicit constexpr MemorySize(T data) noexcept :
				m_data {static_cast<Rep> ((data * Ratio::num) / Ratio::den)}
			{}

			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr MemorySize(const MemorySize<Rep2, Ratio2> &memsize) noexcept {
				*this = memsize;
			}

			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr auto operator=(const MemorySize<Rep2, Ratio2> &memsize) noexcept -> This& {
				m_data = static_cast<Rep> (memsize.m_data);
				return *this;
			}

			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr auto operator+=(const MemorySize<Rep2, Ratio2> &memsize) noexcept -> This& {
				m_data += static_cast<Rep> (memsize.m_data);
				return *this;
			}
			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr auto operator-=(const MemorySize<Rep2, Ratio2> &memsize) noexcept -> This& {
				m_data -= static_cast<Rep> (memsize.m_data);
				return *this;
			}
			template <flex::arithmetic T>
			constexpr auto operator*=(T value) noexcept -> This& {
				m_data = static_cast<Rep> (m_data * value);
				return *this;
			}
			template <flex::arithmetic T>
			constexpr auto operator/=(T value) noexcept -> This& {
				m_data = static_cast<Rep> (m_data / value);
				return *this;
			}

			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr auto operator+(const MemorySize<Rep2, Ratio2> &memsize) const noexcept -> This {
				auto tmp {*this};
				return tmp += memsize;
			}
			template <std::unsigned_integral Rep2, ratio Ratio2>
			constexpr auto operator-(const MemorySize<Rep2, Ratio2> &memsize) const noexcept -> This {
				auto tmp {*this};
				return tmp += memsize;
			}
			template <flex::arithmetic T>
			constexpr auto operator*(T value) const noexcept -> This& {
				auto tmp {*this};
				return tmp *= value;
			}
			template <flex::arithmetic T>
			constexpr auto operator/(T value) const noexcept -> This& {
				auto tmp {*this};
				return tmp /= value;
			}

			explicit constexpr operator Rep() const noexcept {return m_data;}

		private:
			Rep m_data;
	};

	template <std::unsigned_integral Rep, ratio Ratio, flex::arithmetic T>
	constexpr auto operator*(T value, MemorySize<Rep, Ratio> memsize) noexcept
		-> MemorySize<Rep, Ratio>
	{
		return memsize *= value;
	}


	using Byte     = MemorySize<std::size_t, std::ratio<1, 1>>;
	using KiloByte = MemorySize<std::size_t, std::kilo>;
	using MegaByte = MemorySize<std::size_t, std::mega>;
	using GigaByte = MemorySize<std::size_t, std::giga>;
	using KibiByte = MemorySize<std::size_t, std::ratio<1024, 1>>;
	using MebiByte = MemorySize<std::size_t, std::ratio<1024*1024, 1>>;
	using GibiByte = MemorySize<std::size_t, std::ratio<1024*1024*1024, 1>>;


	namespace units::bytes::literals {
		using Integer = unsigned long long;
		using Floating = long double;

		consteval auto operator""_B(Integer value) noexcept {return Byte{value};}
		consteval auto operator""_kB(Integer value) noexcept  {return KiloByte{value};}
		consteval auto operator""_kB(Floating value) noexcept {return KiloByte{value};}
		consteval auto operator""_MB(Integer value) noexcept  {return MegaByte{value};}
		consteval auto operator""_MB(Floating value) noexcept {return MegaByte{value};}
		consteval auto operator""_GB(Integer value) noexcept  {return GigaByte{value};}
		consteval auto operator""_GB(Floating value) noexcept {return GigaByte{value};}
		consteval auto operator""_kiB(Integer value) noexcept  {return KibiByte{value};}
		consteval auto operator""_kiB(Floating value) noexcept {return KibiByte{value};}
		consteval auto operator""_MiB(Integer value) noexcept  {return MebiByte{value};}
		consteval auto operator""_MiB(Floating value) noexcept {return MebiByte{value};}
		consteval auto operator""_GiB(Integer value) noexcept  {return GibiByte{value};}
		consteval auto operator""_GiB(Floating value) noexcept {return GibiByte{value};}
	}


	namespace units::literals {
		using namespace ::vx::units::time::literals;
	}

	namespace literals {
		using namespace ::vx::units::literals;
	}
}
