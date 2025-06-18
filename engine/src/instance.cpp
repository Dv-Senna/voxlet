#include "voxlet/instance.hpp"

#include "voxlet/logger.hpp"
#include "voxlet/utils.hpp"


namespace vx {
	Instance::~Instance() {
		if (!m_built)
			return;
		vx::Logger::global().info("Voxlet instance destroyed");
	}


	auto Instance::create(const CreateInfos &createInfos) noexcept -> vx::Failable<Instance> {
		vx::Logger::global().info("Voxlet instance created. version={}", vx::voxletVersion);
		vx::Logger::global().info("Application : '{}', version={}",
			createInfos.appInfos.name,
			createInfos.appInfos.version
		);
		Instance instance {};
		instance.m_built = true;

		vx::Failable workDirectoryWithError {vx::getExeDirectory()};
		if (!workDirectoryWithError)
			return vx::addErrorToStack(workDirectoryWithError, "Can't get work directory of Voxlet");
		instance.m_workDirectory = std::move(*workDirectoryWithError);

		vx::graphics::Context::CreateInfos graphicsContextCreateInfos {
			.appInfos = createInfos.appInfos
		};
		vx::Failable graphicsContextWithError {vx::graphics::Context::create(graphicsContextCreateInfos)};
		if (!graphicsContextWithError)
			return vx::addErrorToStack(graphicsContextWithError, "Can't create graphics context");
		instance.m_graphicsContext = std::move(*graphicsContextWithError);

		vx::graphics::Window::CreateInfos windowCreateInfos {
			.title = createInfos.appInfos.name,
			.size = {16*70, 9*70}
		};
		vx::Failable windowWithError {vx::graphics::Window::create(windowCreateInfos)};
		if (!windowWithError)
			return vx::addErrorToStack(windowWithError, "Can't create window");
		instance.m_window = std::move(*windowWithError);

		if (auto err {instance.m_graphicsContext.makeWindowCurrent(instance.m_window)}; !err)
			return vx::addErrorToStack(err, "Can't make window current to graphics context");

		return instance;
	}

}
