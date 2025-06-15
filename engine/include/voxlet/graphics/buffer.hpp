#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <flex/bitfield.hpp>
#include <flex/reflection/enums.hpp>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
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


	template <BufferType type>
	class Buffer {
		using This = Buffer<type>;
		Buffer(const This&) = delete;
		auto operator=(const This&) -> This& = delete;

		public:
			constexpr Buffer() noexcept = default;
			constexpr Buffer(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;
			virtual constexpr ~Buffer() = default;

			virtual auto write(vx::Byte offset, std::span<const std::byte> content) noexcept
				-> vx::Failable<void> = 0;
			virtual auto read(vx::Byte offset, std::span<std::byte> buffer) noexcept
				-> vx::Failable<void> = 0;

			virtual auto getSize() const noexcept -> vx::Byte = 0;
	};
}

FLEX_MAKE_ENUM_BITFLAG(vx::graphics::BufferAccess);
