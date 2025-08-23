#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#define VOXLET_CONTAINERS_STRING_EXPOSE_PRIVATE
#include <voxlet/containers/string.hpp>


auto isEmpty(const vx::String& str) -> void {
	REQUIRE(str.isShort());
	REQUIRE(str.empty());
	REQUIRE(str.size() == 0uz);
	REQUIRE(str.capacity() >= str.size());
	for (const auto i : std::views::iota(0uz, str.capacity()))
		REQUIRE(str.getData()[i] == u8'\0');
}

template <std::size_t N>
auto isShort(const vx::String& str, const char8_t (&literal)[N]) -> void {
	REQUIRE(str.isShort());
	REQUIRE(!str.empty());
	REQUIRE(str.size() == N - 1uz);
	REQUIRE(str.capacity() >= str.size());
	for (const auto i : std::views::iota(0uz, str.size()))
		REQUIRE(str.getData()[i] == literal[i]);
}

template <std::size_t N>
auto isLong(const vx::String& str, const char8_t (&literal)[N]) -> void {
	REQUIRE(!str.isShort());
	REQUIRE(!str.empty());
	REQUIRE(str.size() == N - 1uz);
	REQUIRE(str.capacity() == str.size());
	for (const auto i : std::views::iota(0uz, str.size()))
		REQUIRE(str.getData()[i] == literal[i]);
}

auto isEqual(const vx::String& lhs, const vx::String& rhs) -> void {
	REQUIRE(lhs.size() == rhs.size());
	for (const auto i : std::views::iota(0uz, lhs.size()))
		REQUIRE(lhs.getData()[i] == rhs.getData()[i]);
}


TEST_CASE("string", "[containers]") {
	const char8_t shortLiteral[] {u8"Hello World!"};
	const char8_t longLiteral[] {u8"Hello World! I really want this string to be non-SSO, so its long"};

	vx::String emptyStr {};
	vx::String shortStr {vx::String::from(shortLiteral)};
	vx::String longStr {vx::String::from(longLiteral)};

	SECTION("from") {
		isEmpty(emptyStr);
		isShort(shortStr, shortLiteral);
		isLong(longStr, longLiteral);
	}

	SECTION("move") {
		vx::String emptyMove {std::move(emptyStr)};
		isEmpty(emptyStr);
		isEmpty(emptyMove);
		vx::String shortMove {std::move(shortStr)};
		isEmpty(shortStr);
		isShort(shortMove, shortLiteral);
		vx::String longMove {std::move(longStr)};
		isEmpty(longStr);
		isLong(longMove, longLiteral);
	}

	SECTION("copy") {
		vx::String emptyCopy {emptyStr.copy()};
		isEmpty(emptyStr);
		isEmpty(emptyCopy);
		vx::String shortCopy {shortStr.copy()};
		isShort(shortStr, shortLiteral);
		isShort(shortCopy, shortLiteral);
		vx::String longCopy {longStr.copy()};
		isLong(longStr, longLiteral);
		isLong(longCopy, longLiteral);
	}

	SECTION("iteration") {
		for (auto& c : emptyStr)
			++c;
		for (auto& c : shortStr)
			++c;
		for (auto& c : longStr)
			++c;

		REQUIRE(std::ranges::equal(emptyStr, std::vector<char8_t> {}));
		REQUIRE(std::ranges::equal(shortStr, std::span{shortLiteral, sizeof(shortLiteral) - 1uz}
			| std::views::transform([](const auto c) {return c + 1;})
		));
		REQUIRE(std::ranges::equal(longStr, std::span{longLiteral, sizeof(longLiteral) - 1uz}
			| std::views::transform([](const auto c) {return c + 1;})
		));
	}

	SECTION("sliced iteration") {
		REQUIRE(std::ranges::equal(emptyStr.slice(0uz), std::vector<char8_t> {}));
		REQUIRE(std::ranges::equal(shortStr.slice(3uz, 10uz), std::span{shortLiteral + 3uz, 7uz}));
		REQUIRE(std::ranges::equal(longStr.slice(3uz, 10uz), std::span{longLiteral + 3uz, 7uz}));
	}

	SECTION("unchecked iteration") {
		REQUIRE(std::ranges::equal(emptyStr.unchecked(), std::vector<char8_t> {}));
		REQUIRE(std::ranges::equal(shortStr.unchecked(), std::span{shortLiteral, sizeof(shortLiteral) - 1uz}));
		REQUIRE(std::ranges::equal(longStr.unchecked(), std::span{longLiteral, sizeof(longLiteral) - 1uz}));
	}

	SECTION("operator[]") {
		for (const auto i : std::views::iota(0uz, shortStr.size())) {
			++shortStr[i];
			REQUIRE(shortStr.unchecked()[i] == shortLiteral[i] + 1);
		}
		for (const auto i : std::views::iota(0uz, longStr.size())) {
			++longStr[i];
			REQUIRE(longStr.unchecked()[i] == longLiteral[i] + 1);
		}

		for (const auto i : std::views::iota(0uz, 7uz))
			REQUIRE(shortStr.slice(3uz, 10uz)[i] == shortLiteral[i + 3uz] + 1);
		for (const auto i : std::views::iota(0uz, 7uz))
			REQUIRE(longStr.slice(3uz, 10uz)[i] == longLiteral[i + 3uz] + 1);
	}
}
