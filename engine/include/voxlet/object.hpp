#pragma once

#include <concepts>
#include <type_traits>

#include "voxlet/error.hpp"


namespace vx {
	/**
	 * @defgroup object Object noexcept idiom
	 * @brief A way to define and create object to remove the need for exception
	 *        as a error-handling pattern in constructor
	 *
	 * We call an *object* in Voxlet a type `T` such that :
	 *     - `typename T::CreateInfos` is defined and is an aggregate
	 *     - `T::create(<some ref form of typename T::CreateInfos>)` return a
	 *       new instance of `T` as the intended way to construct object (1), or
	 *       return a `vx::Failable<T>` if the construction can fail (2)
	 *     - `T` is move-constructible
	 *
	 * This notion allows for creation of instance of class without the classical
	 * limitation of constructor, that is not being able to communicate failure
	 * in a way other than exception, out parameter or vegetable-state with an
	 * error gettor (and those options are bad, as most construction failure
	 * aren't exceptional failure and should be able to be integrated in the normal
	 * flow of the program).
	 *
	 * With this idiom, the normal way of creating object become
	 * @code{.cpp}
	 * // setup the create infos
	 * MyObject::CreateInfos createInfos {};
	 * // ...
	 *
	 * // non-failable construction (1)
	 * auto myObject {MyObject::create(createInfos)};
	 *
	 * // failable construction (2)
	 * vx::Failable myObjectWithError {MyObject::create(createInfos)};
	 * if (!myObjectWithError)
	 *     // error handling ...
	 * auto myObject (std::move(*myObjectWithError));
	 * @endcode
	 *
	 * which, for (2), makes the error-handling mandatory, as unwrapping an
	 * unchecked `std::expected` is bad pratic at least, can cause catastrophic
	 * problem at worst.
	 * */

	namespace __internals {
		template <typename Ret, typename Obj>
		concept object_create_return = std::same_as<Ret, vx::Failable<Obj>>
			|| std::same_as<Ret, Obj>;
	}

	/**
	 * @brief A concept that implements *object*
	 * @ingroup object
	 * */
	template <typename T>
	concept object = (requires(typename T::CreateInfos createInfos) {
		typename T::CreateInfos;
		{T::create(createInfos)} -> __internals::object_create_return<T>;
	} || requires(typename T::CreateInfos createInfos) {
		typename T::CreateInfos;
		{T::create(std::move(createInfos))} -> __internals::object_create_return<T>;
	})
		&& std::move_constructible<T>
		&& std::is_aggregate_v<typename T::CreateInfos>;


	/**
	 * @brief A utility that allow for the creation of a *is-build* flag for
	 *        *object*. This wrapper handles automaticly the motion of the *object*
	 * @ingroup object
	 * */
	class BuiltFlag {
		public:
			constexpr BuiltFlag() noexcept : m_value {false} {}
			/**
			 * @brief Construct the flag with a value
			 * @param value The value of the flag
			 * */
			constexpr BuiltFlag(bool value) noexcept : m_value {value} {}
			/**
			 * @brief Assign the flag with a value
			 * @param value The value of the flag
			 * */
			constexpr auto operator=(bool value) noexcept -> BuiltFlag& {
				m_value = value;
				return *this;
			}
			/**
			 * @brief Default-construct-copy the flag
			 * */
			constexpr BuiltFlag(const BuiltFlag&) noexcept = default;
			/**
			 * @brief Default-assign-copy the flag
			 * */
			constexpr auto operator=(const BuiltFlag&) noexcept -> BuiltFlag& = default;
			/**
			 * @brief Move the flag value and reset the old flag to `false`
			 * @param flag The flag to move from
			 * */
			constexpr BuiltFlag(BuiltFlag &&flag) noexcept : m_value {flag.m_value} {
				flag.m_value = false;
			}
			/**
			 * @brief Move the flag value and reset the old flag to `false`
			 * @param flag The flag to move from
			 * @return A reference to the current instance
			 * */
			constexpr auto operator=(BuiltFlag &&flag) noexcept -> BuiltFlag& {
				m_value = flag.m_value;
				flag.m_value = false;
				return *this;
			}
			/**
			 * @brief Allow the use of the flag as a good old boolean
			 * @return The value of the flag
			 * */
			explicit constexpr operator bool() const noexcept {return m_value;}

		private:
			bool m_value;
	};
}
