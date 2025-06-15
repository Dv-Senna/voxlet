#pragma once

#include <cstdint>
#include <vector>


namespace vx::graphics {
	enum class PipelineAttributeType : std::uint8_t {
		eFloat,
		eInt32
	};

	struct PipelineAttribute {
		std::uint8_t location;
		std::uint8_t dimension;
		PipelineAttributeType type;
	};

	struct PipelineDescriptor {
		std::vector<PipelineAttribute> attributes;
	};
}
