#pragma once

#include <glad/glad.h>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/graphics/shaderModule.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics::opengl {
	class ShaderModuleBase final {
		ShaderModuleBase(const ShaderModuleBase&) = delete;
		auto operator=(const ShaderModuleBase&) -> ShaderModuleBase& = delete;

		public:
			inline ShaderModuleBase() noexcept = default;
			inline ShaderModuleBase(ShaderModuleBase&&) noexcept = default;
			inline auto operator=(ShaderModuleBase&&) noexcept -> ShaderModuleBase& = default;
			VOXLET_CORE ~ShaderModuleBase();

			struct CreateInfos : public vx::graphics::ShaderModuleDescriptor {
				vx::graphics::ShaderModuleStage stage;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept
				-> vx::Failable<ShaderModuleBase>;

			inline auto getSourcePath() const noexcept -> const std::filesystem::path& {return m_sourcePath;}
			inline auto getEntryPoint() const noexcept -> std::string_view {return m_entryPoint;}

		private:
			vx::BuiltFlag m_built;
			GLuint m_shaderModule;
			std::filesystem::path m_sourcePath;
			std::string m_entryPoint;
			vx::graphics::ShaderModuleStage m_stage;
	};
}
