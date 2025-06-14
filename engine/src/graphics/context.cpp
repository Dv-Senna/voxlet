#include "voxlet/graphics/context.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "voxlet/graphics/opengl/buffer.hpp"
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

		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4))
			return vx::makeErrorStack("Can't set SDL3 OpenGL context major version : {}", SDL_GetError());
		if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6))
			return vx::makeErrorStack("Can't set SDL3 OpenGL context minor version : {}", SDL_GetError());
		vx::Logger::global().info("Voxlet graphics context created");
		return context;
	}


	auto Context::makeWindowCurrent(vx::graphics::Window &window) noexcept -> vx::Failable<void> {
		m_context = SDL_GL_CreateContext(window.getWindow());
		if (m_context == nullptr)
			return vx::makeErrorStack("Can't create an SDL3 OpenGL context : {}", SDL_GetError());
		if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0)
			return vx::makeErrorStack("Can't load GLAD : {}", SDL_GetError());

		int majorVersion {};
		int minorVersion {};
		(void)SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
		(void)SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);

		vx::Logger::global().info("OpenGL context created with version {}.{}", majorVersion, minorVersion);
		return {};
	}
}
