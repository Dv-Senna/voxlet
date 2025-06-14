#pragma once

#include <glad/glad.h>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/graphics/buffer.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics::opengl {
	class BufferBase final {
		BufferBase(const BufferBase&) = delete;
		auto operator=(const BufferBase&) -> BufferBase& = delete;

		public:
			constexpr BufferBase() noexcept = default;
			constexpr BufferBase(BufferBase&&) noexcept = default;
			constexpr auto operator=(BufferBase&&) noexcept -> BufferBase& = default;
			VOXLET_CORE ~BufferBase();

			struct CreateInfos : public vx::graphics::BufferDescriptor {
				vx::graphics::BufferType type;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept
				-> vx::Failable<BufferBase>;

			inline auto getBuffer() const noexcept -> GLuint {return m_buffer;}

		private:
			vx::BuiltFlag m_built;
			GLuint m_buffer;
			flex::Bitfield<BufferAccess> m_access;
	};


	template <vx::graphics::BufferType type>
	class Buffer final : vx::graphics::Buffer<type> {
		using This = Buffer<type>;
		public:
			constexpr Buffer() noexcept = default;
			constexpr Buffer(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;
			constexpr ~Buffer() VOXLET_GRAPHICS_OVERRIDE = default;

			static inline auto create(const vx::graphics::BufferDescriptor &descriptor) noexcept
				-> vx::Failable<This>
			{
				const BufferBase::CreateInfos baseCreateInfos {descriptor, type};
				vx::Failable baseWithError {BufferBase::create(baseCreateInfos)};
				if (!baseWithError)
					return std::unexpected{baseWithError.error()};
				This buffer {};
				buffer.m_base = std::move(*baseWithError);
				return buffer;
			}

		private:
			BufferBase m_base;
	};
}
