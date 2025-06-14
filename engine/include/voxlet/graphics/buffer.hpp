#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <flex/bitfield.hpp>

#include "voxlet/graphics/objectView.hpp"
#include "voxlet/units.hpp"


namespace vx::graphics {
	enum class BufferAccess : std::uint8_t {
		eCpuWritable = 0b01,
		eCpuReadable = 0b10
	};

	enum class BufferType : std::uint8_t {
		eVertex,
		eIndex,
		eShaderStorage,
		eUniform,
		eTransformFeedback
	};

	struct BufferDescriptor {
		vx::Byte size;
		flex::Bitfield<BufferAccess> access;
		std::optional<std::span<const std::byte>> content;
	};

}
