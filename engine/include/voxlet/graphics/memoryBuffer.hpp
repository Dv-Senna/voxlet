#pragma once

#include <cstddef>
#include <optional>
#include <span>

#include <flex/bitfield.hpp>
#include <glad/glad.h>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/units.hpp"


namespace vx::graphics {
	enum class BufferAccess : std::uint8_t {
		eCpuWritable = 0b01,
		eCpuReadable = 0b10
	};

	class Buffer {
		Buffer(const Buffer&) = delete;
		auto operator=(const Buffer&) -> Buffer& = delete;

		public:
			constexpr Buffer() noexcept = default;
			constexpr Buffer(Buffer&&) noexcept = default;
			constexpr auto operator=(Buffer&&) noexcept -> Buffer& = default;
			VOXLET_CORE ~Buffer();

			struct CreateInfos {
				vx::Byte size;
				flex::Bitfield<BufferAccess> access;
				std::optional<std::span<const std::byte>> content;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept
				-> vx::Failable<Buffer>;

			inline auto getBuffer() const noexcept -> GLuint {return m_buffer;}

		private:
			GLuint m_buffer;
			flex::Bitfield<BufferAccess> m_access;
	};
}
