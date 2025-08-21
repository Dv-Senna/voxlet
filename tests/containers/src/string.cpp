#include <ranges>

#include <catch2/catch_test_macros.hpp>

#define VOXLET_CONTAINERS_STRING_EXPOSE_PRIVATE
#include <voxlet/containers/string.hpp>


TEST_CASE("string", "[containers]") {
	SECTION("constructor") {
		vx::String emptyStr {};
		REQUIRE(emptyStr.isShort());
		REQUIRE(emptyStr.empty());
		REQUIRE(emptyStr.size() == 0uz);
		REQUIRE(emptyStr.capacity() >= emptyStr.size());
		for (const auto i : std::views::iota(0uz, emptyStr.capacity()))
			REQUIRE(emptyStr.getData()[i] == u8'\0');

		const char8_t shortLiterals[] {u8"Hello World!"};
		vx::String shortStr {vx::String::from(shortLiterals)};
		REQUIRE(shortStr.isShort());
		REQUIRE(!shortStr.empty());
		REQUIRE(shortStr.size() == sizeof(shortLiterals) - 1uz);
		REQUIRE(shortStr.capacity() >= shortStr.size());
		for (const auto i : std::views::iota(0uz, shortStr.size()))
			REQUIRE(shortStr.getData()[i] == shortLiterals[i]);

		const char8_t longLiterals[] {u8"Hello World! I really want this string to be non-SSO, so its long"};
		vx::String longStr {vx::String::from(longLiterals)};
		REQUIRE(!longStr.isShort());
		REQUIRE(!longStr.empty());
		REQUIRE(longStr.size() == sizeof(longLiterals) - 1uz);
		REQUIRE(longStr.capacity() == longStr.size());
		for (const auto i : std::views::iota(0uz, longStr.size()))
			REQUIRE(longStr.getData()[i] == longLiterals[i]);
	}
}
