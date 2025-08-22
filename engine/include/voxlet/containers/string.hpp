#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>


namespace vx::containers {
	class String final {
		public:
			String(const String&) = delete;
			auto operator=(const String&) -> String& = delete;

			using value_type = char8_t;
			using size_type = std::size_t;

			constexpr String() noexcept;
			constexpr ~String();
			constexpr String(String&& other) noexcept;
			constexpr auto operator=(String&& other) noexcept -> String&;

			template <size_type N>
			static constexpr auto from(const char8_t (&literal)[N]) noexcept -> String;
			static constexpr auto from(const char8_t* raw, size_type N) noexcept -> String;

			constexpr auto copy() const noexcept -> String;

			constexpr auto empty() const noexcept -> bool;
			constexpr auto size() const noexcept -> size_type;
			constexpr auto capacity() const noexcept -> size_type;

		private:
	#ifdef VOXLET_CONTAINERS_STRING_EXPOSE_PRIVATE
		public:
	#endif
			constexpr auto isShort() const noexcept -> bool;
			constexpr auto getData() noexcept -> value_type*;
			constexpr auto getData() const noexcept -> const value_type*;

			static constexpr std::size_t FOOTPRINT {3uz*sizeof(size_type)};
			static constexpr std::size_t SHORT_CAPACITY {FOOTPRINT / sizeof(char8_t) - sizeof(std::uint8_t)};
			static constexpr auto IS_SHORT_MASK {static_cast<std::byte> (0b1000'0000)};
			static constexpr std::size_t LONG_SIZE_MASK {~(static_cast<size_type> (1)
				<< (8uz * sizeof(size_type) - 1uz)
			)};

			static_assert(SHORT_CAPACITY <= std::numeric_limits<std::uint8_t>::max());

			struct LongBigEndian {
				size_type size;
				value_type* data;
				size_type capacity;
			};

			struct ShortBigEndian {
				std::uint8_t size;
				value_type data[SHORT_CAPACITY];
			};

			struct LongLittleEndian {
				value_type* data;
				size_type capacity;
				size_type size;
			};

			struct ShortLittleEndian {
				value_type data[SHORT_CAPACITY];
				std::uint8_t size;
			};

			using Long = std::conditional_t<std::endian::native == std::endian::little,
				LongLittleEndian,
				LongBigEndian
			>;
			using Short = std::conditional_t<std::endian::native == std::endian::little,
				ShortLittleEndian,
				ShortBigEndian
			>;

			union {
				Long m_long;
				Short m_short;
				std::byte m_raw[FOOTPRINT];
			};
	};
}

#include "voxlet/containers/string.inl"

namespace vx {
	using ::vx::containers::String;
}
