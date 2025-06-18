#pragma once

/** @file */

#include <compare>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iterator>
#include <ranges>
#include <string_view>
#include <vector>

#include <flex/pipes/toNumber.hpp>
#include <flex/reference.hpp>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"


namespace vx {
	VOXLET_CORE auto readBinaryFile(const std::filesystem::path &path) noexcept
		-> vx::Failable<std::vector<std::byte>>;
	VOXLET_CORE auto getExePath() noexcept -> vx::Failable<std::filesystem::path>;
	VOXLET_CORE auto getExeDirectory() noexcept -> vx::Failable<std::filesystem::path>;
	VOXLET_CORE auto createDirectory(const std::filesystem::path &path) noexcept
		-> vx::Failable<bool>;
	VOXLET_CORE auto createDirectoryRecursively(const std::filesystem::path &path) noexcept
		-> vx::Failable<bool>;

	using UUID = std::uint64_t;

	/**
	 * @brief Used to produce a compile-time error in consteval context
	 * @ingroup error
	 * */
	auto constevalError(std::string_view msg) noexcept -> void;

	namespace integer::literals {
		/**
		 * @brief An alias to implement the literals more easily
		 * */
		using Number = unsigned long long;
		/**
		 * @brief Construct an integer as a `std::uint8_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_u8(Number number) noexcept {return static_cast<std::uint8_t> (number);}
		/**
		 * @brief Construct an integer as a `std::int8_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_i8(Number number) noexcept {return static_cast<std::int8_t> (number);}
		/**
		 * @brief Construct an integer as a `std::uint16_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_u16(Number number) noexcept {return static_cast<std::uint16_t> (number);}
		/**
		 * @brief Construct an integer as a `std::int16_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_i16(Number number) noexcept {return static_cast<std::int16_t> (number);}
		/**
		 * @brief Construct an integer as a `std::uint32_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_u32(Number number) noexcept {return static_cast<std::uint32_t> (number);}
		/**
		 * @brief Construct an integer as a `std::int32_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_i32(Number number) noexcept {return static_cast<std::int32_t> (number);}
		/**
		 * @brief Construct an integer as a `std::uint64_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_u64(Number number) noexcept {return static_cast<std::uint64_t> (number);}
		/**
		 * @brief Construct an integer as a `std::int64_t`
		 * @param number The number to convert
		 * @return The convert integer
		 * */
		consteval auto operator""_i64(Number number) noexcept {return static_cast<std::int64_t> (number);}
	}


	/**
	 * @brief A struct that represent a version, using semantic versioning
	 * */
	struct Version {
		/**
		 * @brief The major part of the version
		 * */
		std::uint8_t major;
		/**
		 * @brief The minor part of the version
		 * */
		std::uint8_t minor;
		/**
		 * @brief The patch part of the version
		 * */
		std::uint8_t patch;
		/**
		 * @brief Compare the equality of two versions
		 * @return Whether the two versions are equal or not
		 * */
		constexpr auto operator==(const Version&) const noexcept -> bool = default;
		/**
		 * @brief Compare the relative relation between two versions
		 * @param version The version to compare against
		 * @return The ordering, knowing that the biggest version is the most recent
		 * */
		constexpr auto operator<=>(const Version &version) const noexcept -> std::strong_ordering {
			if (major != version.major)
				return major <=> version.major;
			if (minor != version.minor)
				return minor <=> version.minor;
			return patch <=> version.patch;
		}
	};

	/**
	 * @brief The version of Voxlet
	 * */
	constexpr Version voxletVersion {VOXLET_VERSION_MAJOR, VOXLET_VERSION_MINOR, VOXLET_VERSION_PATCH};

	namespace version::literals {
		/**
		 * @brief Create a `vx::Version` from a string literal
		 * @param str Pointer to the string literal start
		 * @param length The length of the string literal
		 * @return The created version
		 * @note This user-defined literal will fail if the version string is
		 *       not valid, ie it's not of the format "M(.m(.p))"
		 * */
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
