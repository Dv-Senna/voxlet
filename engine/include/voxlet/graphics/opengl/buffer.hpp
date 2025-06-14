#pragma once

#include <glad/glad.h>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/graphics/buffer.hpp"


namespace vx::graphics::opengl {
	class Buffer {
		Buffer(const Buffer&) = delete;
		auto operator=(const Buffer&) -> Buffer& = delete;

		public:
			constexpr Buffer() noexcept = default;
			constexpr Buffer(Buffer&&) noexcept = default;
			constexpr auto operator=(Buffer&&) noexcept -> Buffer& = default;
			VOXLET_CORE ~Buffer();

			struct CreateInfos : public vx::graphics::BufferDescriptor {
				BufferType type;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept
				-> vx::Failable<Buffer>;

			inline auto getBuffer() const noexcept -> GLuint {return m_buffer;}

		private:
			GLuint m_buffer;
			flex::Bitfield<BufferAccess> m_access;
	};
}
