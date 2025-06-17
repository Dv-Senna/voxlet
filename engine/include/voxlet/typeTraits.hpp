#pragma once

#include <algorithm>
#include <array>


namespace vx {
	namespace __internals {
		consteval auto hasValueVariadicDuplicates(auto ...values) noexcept -> bool {
			std::array valuesAsArray {values...};
			std::ranges::sort(valuesAsArray);
			return !std::ranges::is_sorted(valuesAsArray, std::ranges::less_equal{});
		}

		consteval auto includeValueVariadic(auto toFind, auto ...values) noexcept -> bool {
			std::array valuesAsArray {values...};
			return std::ranges::find(valuesAsArray, toFind) != valuesAsArray.end();
		}
	}

	template <typename = void, auto ...values>
	struct has_values_duplicate : std::false_type {};

	template <auto ...values>
	struct has_values_duplicate<std::enable_if_t<
		(sizeof...(values) > 0) && vx::__internals::hasValueVariadicDuplicates(values...)
	, void>, values...> : std::true_type {};

	template <auto ...values>
	constexpr auto has_values_duplicate_v = has_values_duplicate<void, values...>::value;


	template <auto value, typename = void, auto ...values>
	struct has_values_value : std::false_type {};

	template <auto value, auto ...values>
	struct has_values_value<value, std::enable_if_t<
		(sizeof...(values) > 0) && vx::__internals::includeValueVariadic(value, values...)
	, void>, values...> : std::true_type {};

	template <auto value, auto ...values>
	constexpr auto has_values_value_v = has_values_value<value, void, values...>::value;
}
