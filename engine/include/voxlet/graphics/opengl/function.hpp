#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

#include <glad/glad.h>

#include "voxlet/error.hpp"
#include "voxlet/enumDispatcher.hpp"


namespace vx::graphics::opengl {
	template <typename Func, typename ...Args, typename Ret = std::invoke_result_t<Func, Args...>>
	[[nodiscard]]
	inline auto call(Func &&func, Args &&...args) noexcept -> vx::Failable<Ret> {
		using namespace std::string_view_literals;
		constexpr vx::EnumDispatcher openglErrorDispatcher {
			std::pair{GL_INVALID_ENUM,                  "invalidEnum"sv},
			std::pair{GL_INVALID_VALUE,                 "invalidValue"sv},
			std::pair{GL_INVALID_OPERATION,             "invalidOperation"sv},
			std::pair{GL_STACK_OVERFLOW,                "stackOverflow"sv},
			std::pair{GL_STACK_UNDERFLOW,               "stackUnderflow"sv},
			std::pair{GL_OUT_OF_MEMORY,                 "outOfMemory"sv},
			std::pair{GL_INVALID_FRAMEBUFFER_OPERATION, "invalidFramebufferOperation"sv},
			std::pair{GL_CONTEXT_LOST,                  "contextLost"sv},
		};

		vx::Failable<Ret> result {};
		if constexpr (std::is_void_v<Ret>)
			func(std::forward<Args> (args)...);
		else
			result = func(std::forward<Args> (args)...);
		if (GLenum firstError {glGetError()}; firstError != GL_NO_ERROR) {
			vx::Failable<Ret> errorStack {vx::makeErrorStack("OpenGL error : {}",
				openglErrorDispatcher(firstError).value_or("<invalid>")
			)};
			for (GLenum error {glGetError()}; error != GL_NO_ERROR; error = glGetError()) {
				errorStack = vx::addErrorToStack(errorStack, "OpenGL error : {}",
					openglErrorDispatcher(error).value_or("<invalid>")
				);
			}
			return errorStack;
		}
		return result;
	}
}
