#pragma once

#include <concepts>
#include <type_traits>

#include "voxlet/error.hpp"


namespace vx {
	struct Object {
		constexpr Object() noexcept = default;
		constexpr Object(Object&&) noexcept = default;

		Object(const Object&) = delete;
		auto operator=(const Object&) -> Object& = delete;
		auto operator=(Object&&) -> Object& = delete;
	};


	namespace __internals {
		template <typename Ret, typename Obj>
		concept object_create_return = std::same_as<Ret, vx::Failable<Obj>>
			|| std::same_as<Ret, Obj>;
	}

	template <typename T>
	concept object = (requires(typename T::CreateInfos createInfos) {
		typename T::CreateInfos;
		{T::create(createInfos)} -> __internals::object_create_return<T>;
	} || requires(typename T::CreateInfos createInfos) {
		typename T::CreateInfos;
		{T::create(std::move(createInfos))} -> __internals::object_create_return<T>;
	})
		&& std::is_base_of_v<Object, T>
		&& std::move_constructible<T>
		&& std::is_aggregate_v<typename T::CreateInfos>;


	class BuiltFlag {
		public:
			constexpr BuiltFlag() noexcept : m_value {false} {}
			constexpr BuiltFlag(bool value) noexcept : m_value {value} {}
			constexpr auto operator=(bool value) noexcept -> BuiltFlag& {
				m_value = value;
				return *this;
			}
			constexpr BuiltFlag(const BuiltFlag&) noexcept = default;
			constexpr auto operator=(const BuiltFlag&) noexcept -> BuiltFlag& = default;
			constexpr BuiltFlag(BuiltFlag &&flag) noexcept : m_value {flag.m_value} {
				flag.m_value = false;
			}
			constexpr auto operator=(BuiltFlag &&flag) noexcept -> BuiltFlag& {
				m_value = flag.m_value;
				flag.m_value = false;
				return *this;
			}
			explicit constexpr operator bool() const noexcept {return m_value;}

		private:
			bool m_value;
	};
}
