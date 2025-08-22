#include <print>
#include <ranges>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <voxlet/containers/string.hpp>


TEST_CASE("string - benchmark", "[containers]") {
	static const char8_t VALID_CHARACTERS[] {u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
	
	const std::size_t size {GENERATE(range(0uz, 32uz))};
	auto randomGenerator {Catch::Generators::random(0uz, sizeof(VALID_CHARACTERS) - 2uz)};
	const auto stringContent = std::views::repeat(0, size)
		| std::views::transform([&randomGenerator](auto) {
			const std::size_t index {randomGenerator.get()};
			(void)randomGenerator.next();
			return VALID_CHARACTERS[index];
		})
		| std::ranges::to<std::vector> ();

	std::println(stderr, "Benchmarking string of size {}", size);

	BENCHMARK_ADVANCED(std::format("[construct from char8_t*] std::u8string - size={}", size))(
		Catch::Benchmark::Chronometer chrono
	) {
		Catch::Benchmark::storage_for<std::u8string> string {};
		chrono.measure([&]() {return string.construct(
			std::from_range, std::span{stringContent.data(), size}
		);});
	};

	BENCHMARK_ADVANCED(std::format("[construct from char8_t*] vx::String - size={}", size))(
		Catch::Benchmark::Chronometer chrono
	) {
		Catch::Benchmark::storage_for<vx::String> string {};
		chrono.measure([&]() {return string.construct(
			vx::String::from(stringContent.data(), size)
		);});
	};
}
