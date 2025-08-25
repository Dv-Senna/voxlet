#include <iterator>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#define VOXLET_CONTAINERS_STRING_EXPOSE_PRIVATE
#include <voxlet/containers/string.hpp>
#include <voxlet/containers/stringAccumulator.hpp>


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


TEST_CASE("string", "[string][containers]") {
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



TEST_CASE("string-accumulator", "[string][containers]") {
	static const char8_t VALID_CHARACTERS[] {u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

	const std::size_t size {GENERATE(128, 1024, 2048)};

	auto characterRandomGenerator {Catch::Generators::random(0uz, sizeof(VALID_CHARACTERS) - 2uz)};
	const auto stringContent = std::views::repeat(0, size)
		| std::views::transform([&characterRandomGenerator](auto) {
			const std::size_t index {characterRandomGenerator.get()};
			(void)characterRandomGenerator.next();
			return VALID_CHARACTERS[index];
		})
		| std::ranges::to<std::vector> ();

	const std::size_t averageSizePerPart {size / 5uz};
	auto sizeRandomGenerator {Catch::Generators::random(
		averageSizePerPart * 9uz / 10uz,
		averageSizePerPart * 11uz / 10uz
	)};
	std::size_t sizeUsed {};
	auto generateNewSize = [&sizeRandomGenerator] {
		const std::size_t size {sizeRandomGenerator.get()};
		(void)sizeRandomGenerator.next();
		return size;
	};

	std::size_t partSize {generateNewSize()};
	const std::u8string stdu8string {std::from_range, stringContent | std::views::take(partSize)};
	sizeUsed += partSize;
	partSize = generateNewSize();
	const auto string {vx::String::from(std::to_address(stringContent.begin() + sizeUsed), partSize)};
	sizeUsed += partSize;
	partSize = generateNewSize();
	const auto stringSlice {vx::String::from(std::to_address(stringContent.begin() + sizeUsed), partSize)};
	sizeUsed += partSize;
	partSize = generateNewSize();
	std::size_t contiguousTakeWhileSize {partSize};
	const auto contiguousNotSizedString {
		std::span{std::to_address(stringContent.begin() + sizeUsed), size - sizeUsed}
		| std::views::take_while([&contiguousTakeWhileSize](auto) {
			return contiguousTakeWhileSize-- != 0uz;
		})
	};
	sizeUsed += partSize;
	std::println("sizeUsed: {}/{}", sizeUsed, size);

	struct InputIterator {
		using difference_type = std::ptrdiff_t;
		using value_type = char8_t;
		inline auto operator==(const std::default_sentinel_t&) const noexcept -> bool {return index == size;}
		inline auto operator++() noexcept -> InputIterator& {++index; return *this;}
		inline auto operator++(int) noexcept -> InputIterator {++index; return *this;}
		inline auto operator*() const noexcept -> char8_t {return value[index];}
		const char8_t* value;
		std::size_t index;
		std::size_t size;
	};
	static_assert(std::input_iterator<InputIterator>);

	auto inputRange {std::ranges::subrange(
		InputIterator{
			std::to_address(stringContent.begin() + sizeUsed),
			0uz,
			size - sizeUsed
		},
		std::default_sentinel
	)};

	vx::containers::StringAccumulator accumulator {};
	accumulator += stdu8string;
	accumulator += string;
	accumulator += stringSlice;
	accumulator += contiguousNotSizedString;
	accumulator += inputRange;

	const auto accumulatorString {accumulator.toString()};

	REQUIRE(std::ranges::equal(accumulatorString, stringContent));
}
