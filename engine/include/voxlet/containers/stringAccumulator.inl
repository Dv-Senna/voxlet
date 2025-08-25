#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <ostream>
#include <ranges>

#include "voxlet/containers/stringAccumulator.hpp"
#include "voxlet/memory.hpp"


namespace vx::containers {
	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::getSize() const noexcept -> std::size_t {
		return m_size;
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::isEmpty() const noexcept -> bool {
		return m_size == 0uz;
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::toString() const noexcept -> vx::String {
		if (this->isEmpty())
			return vx::String{};
		vx::String string {};
		string.resize(m_size);
		char8_t* stringData {std::to_address(string.begin())};

		std::size_t size {m_size};
		const Segment* segment {};
		if constexpr (hasInnerStorage)
			segment = &m_innerSegment;
		else
			segment = m_firstSegment.get();

		while (size != 0uz) {
			assert(segment != nullptr);
			const std::size_t sizeWritten {std::min(size, bufferSize)};
			vx::memory::memcpy(stringData, segment->data, sizeWritten);
			size -= sizeWritten;
			stringData += sizeWritten;
			segment = segment->next.get();
		}
		return string;
	}


	template <std::size_t bufferSize, bool hasInnerStorage>
	template <std::size_t N>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(const char8_t (&literal)[N])
		noexcept
		-> void
	{
		return this->push(literal, N);
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(const char8_t* raw, std::size_t size)
		noexcept
		-> void
	{
		while (size != 0uz) {
			const auto [data, dataSize] {this->reserveMaxOneSegment()};
			vx::memory::memcpy(data, raw, std::min(size, dataSize));
			raw += dataSize;
			size = this->resizeMaxOneSegmentBy(size);
		}
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(const vx::String& string)
		noexcept
		-> void
	{
		return this->push(std::to_address(string.unchecked().begin()), string.size());
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(const vx::StringSlice& slice)
		noexcept
		-> void
	{
		return this->push(std::to_address(slice.unchecked().begin()), slice.size());
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(
		const vx::containers::views::UncheckedStringSlice& slice
	) noexcept -> void {
		return this->push(std::to_address(slice.begin()), slice.size());
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	template <std::ranges::input_range Range>
	requires std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(Range&& range) noexcept -> void {
		for (const char8_t c : std::forward<Range> (range)) {
			const auto [data, _] {this->reserveMaxOneSegment()};
			*data = c;
			(void)this->resizeMaxOneSegmentBy(1uz);
		}
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	template <std::ranges::sized_range Range>
	requires std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(Range&& range) noexcept -> void {
		auto size {static_cast<std::size_t> (std::ranges::size(std::forward<Range> (range)))};
		auto begin {std::ranges::begin(std::forward<Range> (range))};
		const auto end {std::ranges::end(std::forward<Range> (range))};
		while (size != 0uz) {
			const auto [data, dataSize] {this->reserveMaxOneSegment()};
			const std::size_t sizeToTake {std::min(size, dataSize)};
			(void)std::ranges::copy(std::ranges::subrange{begin, end}
				| std::views::take(sizeToTake),
				data
			);
			(void)std::ranges::advance(begin, sizeToTake);
			size = this->resizeMaxOneSegmentBy(size);
		}
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	template <std::ranges::contiguous_range Range>
	requires std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(Range&& range) noexcept -> void {
		auto begin {std::ranges::begin(std::forward<Range> (range))};
		const auto end {std::ranges::end(std::forward<Range> (range))};
		std::size_t remainingSize {bufferSize - (m_size % bufferSize)};
		while (begin != end) {
			auto localEnd {begin};
			const auto rangeSegmentSize {
				remainingSize - static_cast<std::size_t> (std::ranges::advance(localEnd, remainingSize, end))
			};
			std::println("rangeSegmentSize: {}", rangeSegmentSize);
			const auto [data, _] {this->reserveMaxOneSegment()};
			(void)std::ranges::copy(std::ranges::subrange{begin, localEnd}, data);
			this->resizeMaxOneSegmentBy(rangeSegmentSize);
			remainingSize = bufferSize;
			begin = localEnd;
		}
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	template <typename Range>
	requires (std::ranges::contiguous_range<Range>
		&& std::ranges::sized_range<Range>
		&& std::same_as<std::remove_cvref_t<std::ranges::range_value_t<Range>>, char8_t>
	)
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::push(Range&& range) noexcept -> void {
		this->push(
			std::to_address(std::ranges::begin(std::forward<Range> (range))),
			std::ranges::size(std::forward<Range> (range))
		);
	}


	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::reserveMaxOneSegment()
		noexcept
		-> std::pair<char8_t*, std::size_t>
	{
		if (m_lastSegment == nullptr) {
			m_segmentCount = 1uz;
			if constexpr (!hasInnerStorage) {
				m_firstSegment = std::make_unique<Segment> ();
				m_lastSegment = m_firstSegment.get();
			}
			else
				m_lastSegment = &m_innerSegment;
		}
		const std::size_t lastSegmentSize {m_size % bufferSize};
		const std::size_t remainingSize {bufferSize - lastSegmentSize};
		if (remainingSize != 0uz && m_size != m_segmentCount * bufferSize)
			return std::make_pair(m_lastSegment->data + lastSegmentSize, remainingSize);

		m_lastSegment->next = std::make_unique<Segment> ();
		m_lastSegment = m_lastSegment->next.get();
		m_lastSegment->next = nullptr;
		++m_segmentCount;
		return std::make_pair(m_lastSegment->data, bufferSize);
	}

	template <std::size_t bufferSize, bool hasInnerStorage>
	constexpr auto BasicStringAccumulator<bufferSize, hasInnerStorage>::resizeMaxOneSegmentBy(const std::size_t size)
		noexcept
		-> std::size_t
	{
		const std::size_t lastSegmentSize {m_size % bufferSize};
		const std::size_t remainingSize {bufferSize - lastSegmentSize};
		if (remainingSize != 0uz) {
			const std::size_t sizeToAdd {std::min(size, remainingSize)};
			m_size += sizeToAdd;
			return size - sizeToAdd;
		}
		const std::size_t sizeToAdd {std::min(size, bufferSize)};
		m_size += sizeToAdd;
		return size - sizeToAdd;
	}

}
