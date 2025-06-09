#include "voxlet/instance.hpp"

#include "voxlet/logger.hpp"
#include "voxlet/utils.hpp"


namespace vx {
	Instance::~Instance() {
		if (!m_built)
			return;
		vx::Logger::global().verbose("Instance destroyed");
	}


	auto Instance::create(const CreateInfos &createInfos) noexcept -> vx::Failable<Instance> {
		vx::Logger::global().info("Creating instance of voxlet. version={}", vx::voxletVersion);
		vx::Logger::global().info("Application : '{}', version={}", createInfos.appInfos.name, createInfos.appInfos.version);
		Instance instance {};
		instance.m_built = true;
		return instance;
	}

}
