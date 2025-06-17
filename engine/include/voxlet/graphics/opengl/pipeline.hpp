#pragma once

#include <array>

#include <glad/glad.h>

#include "flex/typeTraits.hpp"
#include "voxlet/graphics/opengl/shaderModule.hpp"
#include "voxlet/graphics/pipeline.hpp"
#include "voxlet/graphics/shaderModule.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics::opengl {
	class Pipeline final : public vx::graphics::Pipeline {
		struct ShaderModulePayload {
			vx::graphics::ShaderModuleStage stage;
			GLuint shaderModule;
			vx::UUID uuid;
		};

		template <typename>
		struct stage_extractor {};

		template <vx::graphics::ShaderModuleStage stage>
		struct stage_extractor<vx::graphics::ShaderModule<stage>> : flex::value_constant<stage> {};


		public:
			constexpr Pipeline() = default;
			constexpr Pipeline(Pipeline&&) noexcept = default;
			constexpr auto operator=(Pipeline&&) noexcept -> Pipeline& = default;
			VOXLET_CORE ~Pipeline() override;

			template <vx::graphics::ShaderModuleStage ...stages>
			static auto create(const vx::graphics::PipelineDescriptor<stages...> &createInfos) noexcept
				-> vx::Failable<Pipeline>
			{
				std::tuple narrowShaderModules {std::apply([](auto &...shaderModules) {
					return std::tie(static_cast<
						vx::graphics::opengl::ShaderModule<
							stage_extractor<std::remove_cvref_t<decltype(shaderModules)>>::value
						>&
					> (shaderModules)...);
				}, createInfos.shaderModules)};

				std::array<ShaderModulePayload, sizeof...(stages)> shaderModules {};
				std::apply([&shaderModules](auto &...modules) noexcept {
					shaderModules = std::array{ShaderModulePayload{
						.stage        = modules.getBase().getStage(),
						.shaderModule = modules.getBase().getInternalObject(),
						.uuid         = modules.getUUID()
					}...};
				}, narrowShaderModules);
				return s_create(createInfos.attributes, shaderModules);
			}

			inline auto getUUID() const noexcept -> vx::UUID override {
//				return static_cast<vx::UUID> (0);
			}

		private:
			VOXLET_CORE static auto s_create(
				std::span<const vx::graphics::PipelineAttribute> attributes,
				std::span<const ShaderModulePayload> shaderModules
			) noexcept -> vx::Failable<Pipeline>;

			vx::BuiltFlag m_built;
			GLuint m_program;
			std::vector<ShaderModulePayload> m_shaderModulePayloads;
			std::vector<vx::graphics::PipelineAttribute> m_attributes;
	};
}
