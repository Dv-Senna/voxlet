#include "voxlet/graphics/opengl/buffer.hpp"

#include "flex/reflection/enums.hpp"
#include "voxlet/enumDispatcher.hpp"
#include "voxlet/error.hpp"
#include "voxlet/graphics/buffer.hpp"
#include "voxlet/graphics/opengl/function.hpp"
#include "voxlet/logger.hpp"
#include <cstring>


namespace vx::graphics::opengl {
	BufferBase::~BufferBase() {
		if (!m_built)
			return;
		if (m_buffer == 0)
			return;
		if (m_map != nullptr)
			glUnmapNamedBuffer(m_buffer);
		glDeleteBuffers(1, &m_buffer);
		m_map = nullptr;
		m_buffer = 0;
	}


	auto BufferBase::create(const CreateInfos &createInfos) noexcept
		-> vx::Failable<BufferBase>
	{
		[[maybe_unused]] static const vx::EnumDispatcher bufferTypeDispatcher {
			std::pair{BufferType::eVertex,             GL_ARRAY_BUFFER},
			std::pair{BufferType::eIndex,              GL_ELEMENT_ARRAY_BUFFER},
			std::pair{BufferType::eShaderStorage,      GL_SHADER_STORAGE_BUFFER},
			std::pair{BufferType::eUniform,            GL_UNIFORM_BUFFER},
			std::pair{BufferType::eTransformFeedback,  GL_TRANSFORM_FEEDBACK_BUFFER},
		};

		static const vx::EnumDispatcher bufferAccessDispatcher {
			std::pair{BufferAccess::eCpuReadable, GL_MAP_READ_BIT  | GL_MAP_PERSISTENT_BIT},
			std::pair{BufferAccess::eCpuWritable, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT},
		};

		vx::Failable dataWithError {s_chekContentValidity(createInfos)};
		if (!dataWithError)
			return std::unexpected{dataWithError.error()};
		const std::byte *data  {*dataWithError};

		GLbitfield bufferFlags {};
		for (const auto accessFlag : flex::enum_members_v<vx::graphics::BufferAccess>) {
			if (!(accessFlag.value & createInfos.access))
				continue;
			bufferFlags |= bufferAccessDispatcher(accessFlag.value).value_or(0);
		}

		BufferBase buffer {};
		buffer.m_built = true;
		buffer.m_size = createInfos.size;
		buffer.m_map = nullptr;
		buffer.m_access = createInfos.access;

		glCreateBuffers(1, &buffer.m_buffer);
		auto bufferStorageError {vx::graphics::opengl::call(glNamedBufferStorage,
			buffer.m_buffer,
			static_cast<std::size_t> (buffer.m_size),
			data,
			bufferFlags
		)};
		if (!bufferStorageError) [[unlikely]]
			return vx::addErrorToStack(bufferStorageError, "Can't create buffer storage");

		if (!(bufferFlags & GL_MAP_PERSISTENT_BIT))
			return buffer;

		GLenum mapAccess {};
		if ((buffer.m_access & vx::graphics::BufferAccess::eCpuWritable)
			&& (buffer.m_access & vx::graphics::BufferAccess::eCpuReadable)
		)
			mapAccess = GL_READ_WRITE;
		else if (buffer.m_access & vx::graphics::BufferAccess::eCpuReadable)
			mapAccess = GL_READ_ONLY;
		else
			mapAccess = GL_WRITE_ONLY;
		vx::Failable mapWithError {vx::graphics::opengl::call(glMapNamedBuffer, buffer.m_buffer, mapAccess)}; 
		if (!mapWithError) [[unlikely]]
			return vx::addErrorToStack(mapWithError, "Can't map readable or writable opengl buffer");
		buffer.m_map = static_cast<std::byte*> (*mapWithError);
		return buffer;
	}


	auto BufferBase::s_chekContentValidity(const CreateInfos &createInfos) noexcept
		-> vx::Failable<const std::byte*>
	{
		if (!createInfos.content)
			return nullptr;

		const auto bufferSize {static_cast<std::size_t> (createInfos.size)};
		const auto contentSize {createInfos.content->size_bytes()};
		if (contentSize < bufferSize) [[unlikely]] {
			return vx::makeErrorStack(vx::ErrorCode::eDataTooSmall,
				"Initial buffer data is too small : it must be at least {}B (it is {}B)",
				static_cast<std::size_t> (createInfos.size),
				createInfos.content->size_bytes()
			);
		}

		if (contentSize > bufferSize) [[unlikely]] {
			vx::Logger::global().warning(
				"Given initial buffer data is bigger than the actual buffer : it is {}B instead of {}B",
				createInfos.content->size_bytes(),
				static_cast<std::size_t> (createInfos.size)
			);
		}
		return createInfos.content->data();
	}


	auto BufferBase::write(vx::Byte offset, std::span<const std::byte> content) noexcept
		-> vx::Failable<void>
	{
		if (!(m_access & vx::graphics::BufferAccess::eCpuWritable))
			return vx::makeErrorStack(vx::ErrorCode::eBufferNotWritable);
		if (content.size_bytes() > static_cast<std::size_t> (m_size - offset)) {
			return vx::makeErrorStack(vx::ErrorCode::eDataTooSmall,
				"Can't write to buffer as the content is too small"
			);
		}
		(void)std::memcpy(m_map + static_cast<std::size_t> (offset), content.data(), content.size_bytes());
		return {};
	}

	auto BufferBase::read(vx::Byte offset, std::span<std::byte> buffer) noexcept -> vx::Failable<void> {
		if (!(m_access & vx::graphics::BufferAccess::eCpuWritable))
			return vx::makeErrorStack(vx::ErrorCode::eBufferNotWritable);
		if (buffer.size_bytes() > static_cast<std::size_t> (m_size - offset)) {
			return vx::makeErrorStack(vx::ErrorCode::eDataTooSmall,
				"Can't read from buffer as the buffer is too small"
			);
		}
		(void)std::memcpy(buffer.data(), m_map + static_cast<std::size_t> (offset), buffer.size_bytes());
		return {};
	}
}
