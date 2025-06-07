#pragma once

#include <memory>
#include <span>

#include "voxlet/result.hpp"
#include "voxlet/units.hpp"


namespace vx {
	class Application {
		public:
			constexpr Application() noexcept = default;
			virtual constexpr ~Application() = default;

			virtual auto onStartup() noexcept -> vx::Result = 0;
			virtual auto onUpdate(vx::Milliseconds dt) noexcept -> vx::Result = 0;
			virtual auto onRender() noexcept -> vx::Result = 0;
			virtual auto onShutdown() noexcept -> vx::Result = 0;
	};


	auto createApplication(std::span<char* const> args) noexcept
		-> vx::Failable<std::unique_ptr<Application>>;
}


#ifdef VOXLET_MAIN_FILE
	auto main(int argc, char **argv) -> int {
		vx::Failable appWithError {vx::createApplication(std::span{argv, argc})};
		if (!appWithError)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
#endif
