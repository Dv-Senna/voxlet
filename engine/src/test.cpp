#include "voxlet/test.hpp"

#include <print>


namespace vx {
	auto test() noexcept -> void {
	#ifdef _WIN32
		std::println("Hello World from Voxlet on Windows!");
	#elifdef __linux__
		std::println("Hello World from Voxlet on Linux!");
	#elifdef __APPLE__
		std::println("Hello World from Voxlet on MacOS!");
	#else
		std::println("Hello World from Voxlet on some unknown OS!");
	#endif
	}

} // namespace vx
