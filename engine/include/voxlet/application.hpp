#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "voxlet/error.hpp"
#include "voxlet/units.hpp"
#include "voxlet/utils.hpp"


namespace vx {
	class Instance;

	struct ApplicationInfos {
		std::string_view name;
		vx::Version version;
	};

	class Application {
		public:
			constexpr Application(ApplicationInfos &&infos) noexcept : m_infos {std::move(infos)} {}
			virtual constexpr ~Application() = default;

			virtual auto onStartup(Instance &instance) noexcept -> vx::Failable<void> = 0;
			virtual auto onUpdate(Instance &instance, vx::Milliseconds dt) noexcept -> vx::Failable<void> = 0;
			virtual auto onRender(Instance &instance) noexcept -> vx::Failable<void> = 0;
			virtual auto onShutdown(Instance &instance) noexcept -> vx::Failable<void> = 0;

			constexpr auto getInfos() const noexcept -> const ApplicationInfos& {return m_infos;}

		private:
			ApplicationInfos m_infos;
	};

	auto createApplication(std::span<const std::string_view> args) noexcept
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
