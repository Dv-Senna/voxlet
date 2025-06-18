#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include "voxlet/graphics/shaderModule.hpp"
#include "voxlet/typeTraits.hpp"


namespace vx::graphics {
	template <vx::graphics::ShaderModuleStage ...stages>
	concept compute_pipeline_stages = sizeof...(stages) == 1
		&& stages...[0] == vx::graphics::ShaderModuleStage::eCompute;

	template <vx::graphics::ShaderModuleStage ...stages>
	concept graphics_pipeline_stages = sizeof...(stages) >= 2
		&& !vx::has_values_duplicate_v<stages...>
		&& !vx::has_values_value_v<vx::graphics::ShaderModuleStage::eCompute, stages...>;

	template <vx::graphics::ShaderModuleStage ...stages>
	concept pipeline_stages = compute_pipeline_stages<stages...> != graphics_pipeline_stages<stages...>;

	enum class PipelineAttributeType : std::uint8_t {
		eFloat,
		eInt32
	};

	struct PipelineAttribute {
		std::uint8_t location;
		std::uint8_t dimension;
		std::uint8_t binding;
		PipelineAttributeType type;
	};

	template <vx::graphics::ShaderModuleStage ...stages>
	requires pipeline_stages<stages...>
	struct PipelineDescriptor {
		std::vector<PipelineAttribute> attributes;
		std::tuple<vx::graphics::ShaderModule<stages>&...> shaderModules;
	};

	template <vx::graphics::ShaderModuleStage ...stages>
	PipelineDescriptor(
		std::vector<PipelineAttribute>,
		std::tuple<vx::graphics::ShaderModule<stages>&...>
	) -> PipelineDescriptor<stages...>;


	class Pipeline {
		Pipeline(const Pipeline&) = delete;
		auto operator=(const Pipeline&) -> Pipeline& = delete;

		public:
			constexpr Pipeline() noexcept = default;
			constexpr Pipeline(Pipeline&&) noexcept = default;
			constexpr auto operator=(Pipeline&&) noexcept -> Pipeline& = default;
			virtual constexpr ~Pipeline() = default;
	};
}
