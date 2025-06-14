#include "voxlet/graphics/opengl/buffer.hpp"

#include "voxlet/enumDispatcher.hpp"
#include "voxlet/error.hpp"
#include "voxlet/graphics/opengl/function.hpp"
#include "voxlet/logger.hpp"


namespace vx::graphics::opengl {
	BufferBase::~BufferBase() {
		if (!m_built)
			return;
		if (m_buffer == 0)
			return;
		glDeleteBuffers(1, &m_buffer);
	}


	auto BufferBase::create(const CreateInfos &createInfos) noexcept
		-> vx::Failable<BufferBase>
	{
		[[maybe_unused]] constexpr vx::EnumDispatcher bufferTypeDispatcher {
			std::pair{BufferType::eVertex,             GL_ARRAY_BUFFER},
			std::pair{BufferType::eIndex,              GL_ELEMENT_ARRAY_BUFFER},
			std::pair{BufferType::eShaderStorage,      GL_SHADER_STORAGE_BUFFER},
			std::pair{BufferType::eUniform,            GL_UNIFORM_BUFFER},
			std::pair{BufferType::eTransformFeedback,  GL_TRANSFORM_FEEDBACK_BUFFER},
		};

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

		BufferBase buffer {};
		buffer.m_built = true;
		glCreateBuffers(1, &buffer.m_buffer);
		auto bufferStorageError {vx::graphics::opengl::call(glNamedBufferStorage,
			buffer.m_buffer,
			static_cast<std::size_t> (createInfos.size),
			data,
			0
		)};
		if (!bufferStorageError)
			return vx::addErrorToStack(bufferStorageError, "Can't create buffer storage");
		return buffer;
	}
}
