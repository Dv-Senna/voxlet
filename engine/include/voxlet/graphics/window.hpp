#pragma once

#include <string>
#include <string_view>

#include <SDL3/SDL.h>
#include <turbolin/vector.hpp>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics {
	class Window {
		Window(const Window&) = delete;
		auto operator=(const Window&) -> Window& = delete;

		public:
			constexpr Window() noexcept = default;
			constexpr Window(Window&&) noexcept = default;
			constexpr auto operator=(Window&&) noexcept -> Window& = default;
			VOXLET_CORE ~Window();

			struct CreateInfos {
				std::string_view title;
				tl::Vec2i size;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept -> vx::Failable<Window>;

			inline auto getWindow() const noexcept -> SDL_Window* {return m_window;}

		private:
			vx::BuiltFlag m_built;
			std::string m_title;
			SDL_Window *m_window;
			SDL_GLContext m_context;
	};
}
