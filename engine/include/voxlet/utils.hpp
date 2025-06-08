#pragma once

#include <cstdint>
#include <format>
#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>

#include <flex/pipes/toNumber.hpp>


namespace vx {
	auto constevalError(std::string_view msg) noexcept -> void;

	namespace integer::literals {
		using Number = unsigned long long;
		consteval auto operator""_u8(Number number) noexcept {return static_cast<std::uint8_t> (number);}
		consteval auto operator""_i8(Number number) noexcept {return static_cast<std::int8_t> (number);}
		consteval auto operator""_u16(Number number) noexcept {return static_cast<std::uint16_t> (number);}
		consteval auto operator""_i16(Number number) noexcept {return static_cast<std::int16_t> (number);}
		consteval auto operator""_u32(Number number) noexcept {return static_cast<std::uint32_t> (number);}
		consteval auto operator""_i32(Number number) noexcept {return static_cast<std::int32_t> (number);}
		consteval auto operator""_u64(Number number) noexcept {return static_cast<std::uint64_t> (number);}
		consteval auto operator""_i64(Number number) noexcept {return static_cast<std::int64_t> (number);}
	}


	struct Version {
		std::uint8_t major;
		std::uint8_t minor;
		std::uint8_t patch;
		constexpr auto operator==(const Version&) const noexcept -> bool = default;
	};

	constexpr Version voxletVersion {VOXLET_VERSION_MAJOR, VOXLET_VERSION_MINOR, VOXLET_VERSION_PATCH};

	namespace version::literals {
		consteval auto operator""_ver(const char *str, std::size_t length) noexcept -> Version {
			using namespace ::vx::integer::literals;
			std::string_view versionString {str, length};
			auto splits {versionString
				| std::views::split('.')
				| std::views::transform([](const auto &str) {return std::string_view{str}
					| flex::pipes::to_number<std::uint8_t> ();
				})
				| std::ranges::to<std::vector> ()
			};
			if (splits.empty() || splits.size() > 3uz)
				constevalError("The number of element in a version string must be between 1 and 3 (included)");
			auto validParts {splits
				| std::views::filter([](const auto &opt) {return !!opt;})
			};
			if (static_cast<std::size_t> (std::ranges::distance(validParts)) != splits.size())
				constevalError("Some part(s) of the version string are not valid integer");
			return Version{
				.major = *splits[0],
				.minor = splits.size() > 1 && !!splits[1] ? *splits[1] : 0_u8,
				.patch = splits.size() > 2 && !!splits[2] ? *splits[2] : 0_u8
			};
		}

		static_assert("1"_ver == Version{1, 0, 0});
		static_assert("1.2"_ver == Version{1, 2, 0});
		static_assert("1.2.3"_ver == Version{1, 2, 3});
	}


	namespace literals {
		using namespace ::vx::integer::literals;
		using namespace ::vx::version::literals;
	}
}


template <>
struct std::formatter<vx::Version> {
	constexpr auto parse(std::format_parse_context &ctx) -> std::format_parse_context::iterator {
		return ctx.end() - 1;
	}

	auto format(const vx::Version &version, std::format_context &ctx) const noexcept
		-> std::format_context::iterator
	{
		std::string text {std::format("{}.{}.{}",
			version.major,
			version.minor,
			version.patch
		)};
		return std::ranges::copy(text, ctx.out()).out;
	};
};
