#include "voxlet/instance.hpp"
#include "voxlet/utils.hpp"

#include <print>


namespace vx {
	Instance::~Instance() {
		if (!m_built)
			return;
		std::println("Instance destroyed");
	}


	auto Instance::create(const CreateInfos &createInfos) noexcept -> vx::Failable<Instance> {
		std::println("Creating instance of voxlet. version={}", vx::voxletVersion);
		std::println("Application : '{}', version={}", createInfos.appInfos.name, createInfos.appInfos.version);
		Instance instance {};
		instance.m_built = true;
		return instance;
	}

}
