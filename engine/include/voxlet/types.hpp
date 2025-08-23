#pragma once

#include <cstddef>
#include <limits>


namespace vx::types {
	using Extent = std::size_t;
	constexpr Extent runtime_extent = std::numeric_limits<Extent>::max();

	struct Empty {};
}
