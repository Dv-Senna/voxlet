#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <type_traits>

#include "voxlet/containers/views/checkedContiguousIterator.hpp"
#include "voxlet/containers/views/uncheckedStringSlice.hpp"


namespace vx::containers::views {
	class StringSlice;
}

namespace vx::containers {
	class String final {
		friend class vx::containers::views::StringSlice;

		public:
			String(const String&) = delete;
			auto operator=(const String&) -> String& = delete;

			using value_type = char8_t;
			using size_type = std::size_t;
			using short_size_type = std::uint8_t;
			static constexpr size_type npos = std::numeric_limits<size_type>::max();

			using iterator = vx::containers::views::CheckedContiguousIterator<value_type, String>;
			using const_iterator = vx::containers::views::CheckedContiguousIterator<const value_type, String>;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			friend iterator;
			friend const_iterator;

			constexpr String() noexcept;
			constexpr ~String();
			constexpr String(String&& other) noexcept;
			constexpr auto operator=(String&& other) noexcept -> String&;

			template <size_type N>
			[[nodiscard]]
			static constexpr auto from(const char8_t (&literal)[N]) noexcept -> String;
			[[nodiscard]]
			static constexpr auto from(const char8_t* raw, size_type N) noexcept -> String;
			[[nodiscard]]
			static constexpr auto from(const vx::containers::views::StringSlice& slice) noexcept -> String;
			[[nodiscard]]
			static constexpr auto from(const vx::containers::views::UncheckedStringSlice& slice) noexcept -> String;

			[[nodiscard]]
			constexpr auto copy() const noexcept -> String;
			[[nodiscard]]
			constexpr auto slice() const noexcept -> vx::containers::views::StringSlice;
			[[nodiscard]]
			constexpr auto slice(size_type start, size_type end = npos) const
				noexcept
				-> vx::containers::views::StringSlice;
			[[nodiscard]]
			constexpr auto slice(const_iterator start, std::optional<const_iterator> end = std::nullopt) const
				noexcept
				-> vx::containers::views::StringSlice;
			[[nodiscard]]
			constexpr auto unchecked() const noexcept -> vx::containers::views::UncheckedStringSlice;

			[[nodiscard]]
			constexpr auto operator[](size_type index) noexcept -> value_type&; 
			[[nodiscard]]
			constexpr auto operator[](size_type index) const noexcept -> const value_type&; 

			constexpr auto reserve(size_type newCapacity) noexcept -> void;
			constexpr auto resize(size_type newSize) noexcept -> void;

			[[nodiscard]]
			constexpr auto isEmpty() const noexcept -> bool;
			[[nodiscard]]
			constexpr auto getSize() const noexcept -> size_type;
			[[nodiscard]]
			constexpr auto getCapacity() const noexcept -> size_type;

			[[nodiscard]]
			constexpr auto begin() noexcept -> iterator;
			[[nodiscard]]
			constexpr auto end() noexcept -> iterator;
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			[[nodiscard]]
			constexpr auto cbegin() const noexcept -> const_iterator;
			[[nodiscard]]
			constexpr auto cend() const noexcept -> const_iterator;

			[[nodiscard]]
			constexpr auto rbegin() noexcept -> reverse_iterator;
			[[nodiscard]]
			constexpr auto rend() noexcept -> reverse_iterator;
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}
			[[nodiscard]]
			constexpr auto crbegin() const noexcept -> const_reverse_iterator;
			[[nodiscard]]
			constexpr auto crend() const noexcept -> const_reverse_iterator;

			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto empty() const noexcept -> bool {return this->isEmpty();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto size() const noexcept -> size_type {return this->getSize();}
			[[nodiscard]]
			[[gnu::always_inline]]
			constexpr auto capacity() const noexcept -> size_type {return this->getCapacity();}

		private:
	#ifdef VOXLET_CONTAINERS_STRING_EXPOSE_PRIVATE
		public:
	#endif
			[[nodiscard]]
			constexpr auto isPointerValid(const value_type* ptr) const noexcept -> bool;
			[[nodiscard]]
			constexpr auto isPointerEnd(const value_type* ptr) const noexcept -> bool;

			[[nodiscard]]
			constexpr auto isShort() const noexcept -> bool;
			[[nodiscard]]
			constexpr auto getData() noexcept -> value_type*;
			[[nodiscard]]
			constexpr auto getData() const noexcept -> const value_type*;

			constexpr auto setSize(size_type size) noexcept -> void;

			static constexpr std::size_t FOOTPRINT {3uz*sizeof(size_type)};
			static constexpr std::size_t SHORT_CAPACITY {FOOTPRINT / sizeof(value_type) - sizeof(short_size_type)};
			static constexpr auto IS_SHORT_MASK {static_cast<std::byte> (0b1000'0000)};
			static constexpr std::size_t LONG_SIZE_MASK {~(static_cast<size_type> (1)
				<< (8uz * sizeof(size_type) - 1uz)
			)};

			static_assert(SHORT_CAPACITY <= std::numeric_limits<short_size_type>::max());

			struct LongBigEndian {
				size_type size;
				value_type* data;
				size_type capacity;
			};

			struct ShortBigEndian {
				short_size_type size;
				value_type data[SHORT_CAPACITY];
			};

			struct LongLittleEndian {
				value_type* data;
				size_type capacity;
				size_type size;
			};

			struct ShortLittleEndian {
				value_type data[SHORT_CAPACITY];
				short_size_type size;
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

	static_assert(std::ranges::contiguous_range<String>);
}

#include "voxlet/containers/string.inl"

namespace vx {
	using ::vx::containers::String;
}
