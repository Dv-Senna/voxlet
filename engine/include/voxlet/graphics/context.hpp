#pragma once

#include "voxlet/application.hpp"
#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics {
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

		private:
			vx::BuiltFlag m_built;
	};
}
