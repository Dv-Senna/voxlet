#include "voxlet/graphics/window.hpp"

#include "voxlet/logger.hpp"


namespace vx::graphics {
	Window::~Window() {
		if (!m_built)
			return;
		if (m_window != nullptr)
			SDL_DestroyWindow(m_window);
		vx::Logger::global().verbose("Voxlet window '{}' destroyed", m_title);
	}


	auto Window::create(const CreateInfos &createInfos) noexcept -> vx::Failable<Window> {
		Window window {};
		window.m_built = true;
		window.m_title = createInfos.title;

		window.m_window = SDL_CreateWindow(
			window.m_title.c_str(),
			createInfos.size.x, createInfos.size.y,
			SDL_WINDOW_OPENGL
		);
		if (window.m_window == nullptr)
			return vx::makeErrorStack("Can't create an SDL3 window : {}", SDL_GetError());

		vx::Logger::global().verbose("Voxlet window '{}' of initial size {} created",
			window.m_title,
			createInfos.size
		);
		return window;
	}
}
