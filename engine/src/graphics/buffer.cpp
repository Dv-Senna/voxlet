#include "voxlet/graphics/buffer.hpp"
#include "voxlet/logger.hpp"


namespace vx::graphics {
	Buffer::~Buffer() {
		if (m_buffer == 0)
			return;
		glDeleteBuffers(1, &m_buffer);
	}


	auto Buffer::create(const CreateInfos &createInfos) noexcept
		-> vx::Failable<Buffer>
	{
		const std::byte *data {nullptr};
		if (createInfos.content) {
			if (createInfos.content->size_bytes() < static_cast<std::size_t> (createInfos.size)) {
				return vx::makeErrorStack(vx::ErrorCode::eDataTooSmall,
					"Initial buffer data is too small : it must be at least {}B (it is {}B)",
					static_cast<std::size_t> (createInfos.size),
					createInfos.content->size_bytes()
				);
			}

			if (createInfos.content->size_bytes() > static_cast<std::size_t> (createInfos.size)) {
				vx::Logger::global().warning(
					"Given initial buffer data is bigger than the actual buffer : it is {}B instead of {}B",
					createInfos.content->size_bytes(),
					static_cast<std::size_t> (createInfos.size)
				);
			}

			data = createInfos.content->data();
		}

		Buffer buffer {};
		glCreateBuffers(1, &buffer.m_buffer);
		glNamedBufferStorage(
			buffer.m_buffer,
			static_cast<std::size_t> (createInfos.size),
			data,
			0
		);

		return buffer;
	}
}
