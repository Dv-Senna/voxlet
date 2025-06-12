#pragma once

#include <SDL3/SDL.h>

#include "voxlet/application.hpp"
#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics {
	class Window;

	class Context {
		Context(const Context&) = delete;
		auto operator=(const Context&) -> Context& = delete;

		public:
			constexpr Context() noexcept = default;
			constexpr Context(Context&&) noexcept = default;
			constexpr auto operator=(Context&&) noexcept -> Context& = default;
			VOXLET_CORE ~Context();

			struct CreateInfos {
				const vx::ApplicationInfos &appInfos;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept -> vx::Failable<Context>;
			VOXLET_CORE auto makeWindowCurrent(vx::graphics::Window &window) noexcept -> vx::Failable<void>;

			inline auto getContext() const noexcept -> SDL_GLContext {return m_context;};

		private:
			vx::BuiltFlag m_built;
			SDL_GLContext m_context;
	};
}
