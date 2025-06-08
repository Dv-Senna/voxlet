#pragma once

#include <span>
#include <string_view>

#include "voxlet/application.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx {
	class Instance final : vx::Object {
		public:
			~Instance();
			constexpr Instance(Instance&&) noexcept = default;

			struct CreateInfos {
				const vx::ApplicationInfos &appInfos;
				const std::span<const std::string_view> args;
			};

			static auto create(const CreateInfos &createInfos) noexcept -> vx::Failable<Instance>;

		private:
			constexpr Instance() noexcept = default;

			vx::BuiltFlag m_built;
	};

	static_assert(vx::object<Instance>);
}
