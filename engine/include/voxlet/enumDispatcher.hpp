#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <optional>
#include <utility>

#include <flex/reflection/enums.hpp>
#include <flex/typeTraits.hpp>

#include "voxlet/utils.hpp"


namespace vx {
	template <typename Key, typename Value, std::size_t N>
	class EnumDispatcher final {
		using This = EnumDispatcher<Key, Value, N>;
		EnumDispatcher() = delete;
		EnumDispatcher(const This&) = delete;
		auto operator=(const This&) -> This& = delete;
		EnumDispatcher(This&&) = delete;
		auto operator=(This&&) -> This& = delete;

		public:
			template <flex::pair ...Args>
			requires (
				(std::same_as<typename Args::first_type, Key>
				&& std::convertible_to<typename Args::second_type, Value>) && ...
			) && (sizeof...(Args) == N)
			consteval EnumDispatcher(Args ...args) noexcept {
				m_values = std::array{std::pair{args.first, static_cast<Value> (args.second)}...};
				std::ranges::sort(m_values, {}, &std::pair<Key, Value>::first);
				if (!std::ranges::is_sorted(
					m_values,
					std::ranges::less_equal{},
					&std::pair<Key, Value>::first
				))
					vx::constevalError("Can't have duplicated keys in vx::EnumDispatcher");
			}

			constexpr auto operator()(Key key) const noexcept -> std::optional<Value> {
				auto it {std::ranges::lower_bound(m_values, key, {}, &std::pair<Key, Value>::first)};
				if (it == m_values.end())
					return std::nullopt;
				const auto index {static_cast<std::size_t> (std::distance(m_values.begin(), it))};
				return m_values[index].second;
			}

		private:
			std::array<std::pair<Key, Value>, N> m_values;
	};


	template <flex::pair ...Args>
	requires (sizeof...(Args) > 0)
	EnumDispatcher(Args ...args) -> EnumDispatcher<
		typename Args...[0]::first_type,
		typename Args...[0]::second_type,
		sizeof...(Args)
	>;
}
