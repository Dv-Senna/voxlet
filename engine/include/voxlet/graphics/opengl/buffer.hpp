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

			VOXLET_CORE auto write(vx::Byte offset, std::span<const std::byte> content) noexcept
				-> vx::Failable<void>;
			VOXLET_CORE auto read(vx::Byte offset, std::span<std::byte> buffer) noexcept
				-> vx::Failable<void>;

			inline auto getBuffer() const noexcept -> GLuint {return m_buffer;}
			inline auto getSize() const noexcept -> vx::Byte {return m_size;}

		private:
			VOXLET_CORE static auto s_chekContentValidity(const CreateInfos &createInfos) noexcept
				-> vx::Failable<const std::byte*>;

			vx::BuiltFlag m_built;
			GLuint m_buffer;
			vx::Byte m_size;
			flex::Bitfield<BufferAccess> m_access;
			std::byte *m_map;
	};


	template <vx::graphics::BufferType type>
	class Buffer final : vx::graphics::Buffer<type> {
		using This = Buffer<type>;
		public:
			constexpr Buffer() noexcept = default;
			constexpr Buffer(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;
			constexpr ~Buffer() override = default;

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

			inline auto write(vx::Byte offset, std::span<const std::byte> content) noexcept
				-> vx::Failable<void> override {return m_base.write(offset, content);}
			inline auto read(vx::Byte offset, std::span<std::byte> content) noexcept
				-> vx::Failable<void> override {return m_base.read(offset, content);}

			inline auto getSize() const noexcept -> vx::Byte override {return m_base.getSize();}

		private:
			BufferBase m_base;
	};
}
