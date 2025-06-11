#include "voxlet/graphics/context.hpp"

#include <SDL3/SDL.h>

#include "voxlet/logger.hpp"


namespace vx::graphics {
	Context::~Context() {
		if (!m_built)
			return;
		SDL_Quit();
		vx::Logger::global().info("Voxlet graphics context destroyed");
	}

	auto Context::create([[maybe_unused]] const CreateInfos &createInfos) noexcept -> vx::Failable<Context> {
		Context context {};
		if (!SDL_Init(SDL_INIT_VIDEO))
			return vx::makeErrorStack("Can't init SDL3 : {}", SDL_GetError());
		context.m_built = true;
		vx::Logger::global().info("Voxlet graphics context created");
		return context;
	}
}
