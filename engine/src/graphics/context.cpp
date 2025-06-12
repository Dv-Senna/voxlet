#include "voxlet/graphics/context.hpp"

#include <SDL3/SDL.h>

#include "voxlet/graphics/window.hpp"
#include "voxlet/logger.hpp"


namespace vx::graphics {
	Context::~Context() {
		if (!m_built)
			return;
		if (m_context != nullptr)
			SDL_GL_DestroyContext(m_context);
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


	auto Context::makeWindowCurrent(vx::graphics::Window &window) noexcept -> vx::Failable<void> {
		m_context = SDL_GL_CreateContext(window.getWindow());
		if (m_context == nullptr)
			return vx::makeErrorStack("Can't create an SDL3 OpenGL context : {}", SDL_GetError());
		return {};
	}
}
