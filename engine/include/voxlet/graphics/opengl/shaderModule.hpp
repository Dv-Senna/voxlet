#pragma once

#include <utility>

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
			inline auto getInternalObject() const noexcept -> GLuint {return m_shaderModule;}
			inline auto getStage() const noexcept -> vx::graphics::ShaderModuleStage {return m_stage;}

		private:
			vx::BuiltFlag m_built;
			GLuint m_shaderModule;
			std::filesystem::path m_sourcePath;
			std::string m_entryPoint;
			vx::graphics::ShaderModuleStage m_stage;
	};


	template <vx::graphics::ShaderModuleStage stage>
	class ShaderModule final : public vx::graphics::ShaderModule<stage> {
		using This = ShaderModule<stage>;
		public:
			inline ShaderModule() noexcept = default;
			inline ShaderModule(This&&) noexcept = default;
			inline auto operator=(This&&) noexcept -> This& = default;
			inline ~ShaderModule() override = default;

			static inline auto create(const vx::graphics::ShaderModuleDescriptor &descriptor) noexcept
				-> vx::Failable<This>
			{
				const ShaderModuleBase::CreateInfos baseCreateInfos {descriptor, stage};
				vx::Failable baseWithError {ShaderModuleBase::create(baseCreateInfos)};
				if (!baseWithError)
					return std::unexpected{baseWithError.error()};
				This shaderModule {};
				shaderModule.m_base = std::move(*baseWithError);
				return shaderModule;
			}

			inline auto getSourcePath() const noexcept -> const std::filesystem::path& override {
				return m_base.getSourcePath();
			}
			inline auto getEntryPoint() const noexcept -> std::string_view override {
				return m_base.getEntryPoint();
			}

			inline auto getBase() noexcept -> ShaderModuleBase& {return m_base;}


		private:
			ShaderModuleBase m_base;
	};
}
