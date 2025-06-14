#pragma once

#include <optional>
#include <type_traits>

#include <flex/reference.hpp>

#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx::graphics {
	template <vx::object Obj>
	auto getObjectSpecifier(Obj &obj) noexcept;
	constexpr auto isSameObjectSpecifier(auto lhs, auto rhs) noexcept -> bool {
		return lhs == rhs;
	}

	template <vx::object Obj, auto spec>
	class ObjectView {
		using This = ObjectView<Obj, spec>;
		public:
			constexpr ObjectView() noexcept = default;
			constexpr ~ObjectView() = default;
			constexpr ObjectView(const This&) noexcept = default;
			constexpr auto operator=(const This&) noexcept -> This& = default;
			constexpr ObjectView(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;

			template <vx::object Obj2>
			requires std::is_base_of_v<Obj, Obj2>
			static constexpr auto create(Obj2 &object) noexcept -> vx::Failable<This> {
				if (!isSameObjectSpecifier(getObjectSpecifier(object), spec))
					return vx::makeErrorStack(vx::ErrorCode::eInvalidObjectViewSpec);
				This view {};
				view.m_object = object;
				return view;
			}

			constexpr auto holdValue() const noexcept -> bool {return !!m_object;}
			constexpr explicit operator bool() const noexcept {return this->holdValue();}
			constexpr auto operator*() const noexcept -> Obj& {return *m_object;}
			constexpr auto operator->() const noexcept -> Obj* {return &*m_object;}
			constexpr operator Obj&() const noexcept {return **this;}

		protected:
			std::optional<flex::Reference<Obj>> m_object;
	};
}
