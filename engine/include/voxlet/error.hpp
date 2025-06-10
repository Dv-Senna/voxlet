#pragma once

#include <expected>
#include <format>
#include <optional>
#include <print>
#include <source_location>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <utility>

#include <flex/pipes/toString.hpp>
#include <flex/pipes/transform.hpp>
#include <flex/pipes/valueOr.hpp>


namespace vx {
	/**
	 * @defgroup error Error / error handling
	 * @brief Every error or error handling related stuff
	 *
	 * Here we define a few term related to how errors work in Voxlet :
	 *     - We call *error-frame* the object that contains the data about a
	 *       specific error at a specific point in space or time. This usually
	 *       contains a message, some `vx::ErrorCode` and the function where
	 *       the error appeared
	 *     - We call *error-stack* the stack-like collection of all *error-frame*,
	 *       such that the top of the stack is the latest / most broad error in
	 *       the callstack, and the bottom is the original / most specific error
	 *     - We call *error-payload* the object that is transmited between
	 *       functions on the callstack. In our case, this can be understood as
	 *       the *error-stack*
	 *     - We call *failable* the object that allow the seemless transmition
	 *       of either the success return value, or the *error-payload* in case
	 *       of a failure
	 *
	 * As a general rule, try to handle the error as soon as possible. But if
	 * you can't do anything from where you are, push your error on the
	 * *erro-stack* carried by the *error-payload* and propagate-it to the next
	 * function.
	 * */

	/**
	 * @brief Different error code that a function can return to be more specific
	 *        about the cause of failure
	 * @ingroup error
	 * @sa vx::ErrorFrame
	 * */
	enum class ErrorCode : std::uint32_t {
		/**
		 * @brief Indicate a general failure. Use only if nothing better is
		 *        available, as it will most likely result in a fatal error
		 * */
		eFailure,
	};

	/**
	 * @brief A struct representing an *error-frame*
	 * @ingroup error
	 * @sa vx::ErrorCode
	 * @sa vx::ErrorPayload
	 * */
	struct [[nodiscard]] ErrorFrame {
		/**
		 * @brief An optional human-readable message for better error understanding
		 * */
		std::optional<std::string> message;
		/**
		 * @brief An optional error code for better handling of the error
		 * */
		std::optional<ErrorCode> errorCode;
		/**
		 * @brief The location of the error
		 * */
		std::source_location location;
	};


	/**
	 * @brief Some iterator class to allow for iteration on the
	 *        `vx::ErrorPayload`'s *error-stack*
	 * @ingroup error
	 * @sa vx::ErrorPayload
	 * */
	class ErrorPayloadIterator {
		friend struct ErrorPayload;
		public:
			/**
			 * @brief Default construct / end tag
			 * */
			constexpr ErrorPayloadIterator() noexcept : m_frames {nullptr} {};
			/**
			 * @brief Dereference operator
			 * @return A reference to the top-most `vx::ErrorFrame`
			 * */
			inline auto operator*() const noexcept -> auto& {return m_frames->top();}
			/**
			 * @brief Dereference operator
			 * @return A pointer to the top-most `vx::ErrorFrame`
			 * */
			inline auto operator->() const noexcept {return &m_frames->top();}
			/**
			 * @brief Pop the top-most `vx::ErrorFrame` out of the `vx::ErrorPayload`
			 * @warning This has a permanent forgetting effect on the *error-payload*
			 * @return A reference to the iterator
			 * */
			inline auto operator++() noexcept -> ErrorPayloadIterator& {
				if (m_frames != nullptr)
					m_frames->pop();
				return *this;
			}

			/**
			 * @brief Check iterator equality
			 * @param it An iterator to check to `this`
			 * @return Whether the two iterators are equivalent or not
			 * */
			inline auto operator==(const ErrorPayloadIterator &it) const noexcept -> bool {
				if (m_frames == nullptr && it.m_frames == nullptr)
					return true;
				if (m_frames == nullptr && it.m_frames->empty())
					return true;
				if (it.m_frames == nullptr && m_frames->empty())
					return true;
				return m_frames == it.m_frames;
			}

		private:
			inline ErrorPayloadIterator(std::stack<ErrorFrame> &frame) noexcept : m_frames {&frame} {}
			std::stack<ErrorFrame> *m_frames;
	};

	/**
	 * @brief A struct representing an *error-payload*
	 * @note You don't usually interact with this struct directly, but through
	 *       `vx::makeErrorStack()` and `vx::addErrorToStack()`
	 * @ingroup error
	 * @sa vx::ErrorFrame
	 * @sa vx::makeErrorStack()
	 * @sa vx::addErrorToStack()
	 * @sa vx::ErrorPayloadIterator
	 * */
	struct [[nodiscard]] ErrorPayload {
		/**
		 * @brief The actual *error-stack*
		 * */
		std::stack<ErrorFrame> frames;
		/**
		 * @brief Begin iterator of payload. Is always equivalent to all
		 *        non-ending iterator on an instance of the *error-payload*
		 * @return An iterator
		 * */
		inline auto begin() noexcept {return ErrorPayloadIterator{frames};}
		/**
		 * @brief End iterator of payload
		 * @return An ending iterator
		 * */
		inline auto end() const noexcept {return ErrorPayloadIterator{};}
	};

	/**
	 * @brief Create an *error-stack* with a single *error-frame*, and wrap-it
	 *        in an *error-payload*
	 * @param location The location where the error happened. Provided by macros
	 * @param format The format string of a message
	 * @param args The argument of the format string
	 * @return An *error-payload* containing the created *error-stack*
	 * @ingroup error
	 * @sa vx::ErrorPayload
	 * @sa vx::ErrorFrame
	 * @sa vx::addErrorToStack()
	 * */
	template <typename ...Args>
	[[nodiscard]]
	constexpr auto makeErrorStack(
		std::source_location &&location,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		ErrorPayload payload {};
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.errorCode = std::nullopt,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	/**
	 * @overload
	 * @param location The location where the error happened. Provided by macros
	 * @param errorCode The code of the error
	 * @param format The format string of a message
	 * @param args The argument of the format string
	 * @ingroup error
	 * */
	template <typename ...Args>
	[[nodiscard]]
	constexpr auto makeErrorStack(
		std::source_location &&location,
		ErrorCode errorCode,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		ErrorPayload payload {};
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.errorCode = errorCode,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	/**
	 * @overload
	 * @param location The location where the error happened. Provided by macros
	 * @param errorCode The code of the error
	 * @ingroup error
	 * */
	[[nodiscard]]
	constexpr auto makeErrorStack(
		std::source_location &&location,
		ErrorCode errorCode
	) noexcept {
		ErrorPayload payload {};
		payload.frames.push(ErrorFrame{
			.message = std::nullopt,
			.errorCode = errorCode,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	/**
	 * @brief Add an *error-frame* to an *error-stack* and wrap-it in an *error-payload*
	 * @param location The location where the error happened. Provided by macros
	 * @param payload The *error-payload* containing the *error-stack* to add to
	 * @param format The format string of a message
	 * @param args The argument of the format string
	 * @return An rvalue reference to the *error-payload*
	 * @ingroup error
	 * @sa vx::ErrorPayload
	 * @sa vx::ErrorFrame
	 * @sa vx::makeErrorStack()
	 * */
	template <typename ...Args>
	[[nodiscard]]
	constexpr auto addErrorToStack(
		std::source_location &&location,
		ErrorPayload &payload,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.errorCode = std::nullopt,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	/**
	 * @overload
	 * @param location The location where the error happened. Provided by macros
	 * @param payload The *error-payload* containing the *error-stack* to add to
	 * @param errorCode The code of the error
	 * @param format The format string of a message
	 * @param args The argument of the format string
	 * @ingroup error
	 * */
	template <typename ...Args>
	[[nodiscard]]
	constexpr auto addErrorToStack(
		std::source_location &&location,
		ErrorPayload &payload,
		ErrorCode errorCode,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.errorCode = errorCode,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	/**
	 * @overload
	 * @param location The location where the error happened. Provided by macros
	 * @param payload The *error-payload* containing the *error-stack* to add to
	 * @param errorCode The code of the error
	 * @ingroup error
	 * */
	[[nodiscard]]
	constexpr auto addErrorToStack(
		std::source_location &&location,
		ErrorPayload &payload,
		ErrorCode errorCode
	) noexcept {
		payload.frames.push(ErrorFrame{
			.message = std::nullopt,
			.errorCode = errorCode,
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}


	/**
	 * @brief A wrapper that return either the return value of a function, or an `vx::ErrorPayload`
	 * @tparam T The type of the return value of a function on success
	 *
	 * This wrapper is more or less an alias to std::expected, but with the `[[nodiscard]]` added.
	 *
	 * @ingroup error
	 * @sa vx::ErrorPayload
	 * @sa vx::makeErrorStack()
	 * @sa vx::addErrorToStack()
	 * */
	template <typename T>
	struct [[nodiscard]] Failable : public std::expected<T, ErrorPayload> {
		using std::expected<T, ErrorPayload>::expected;
	};
}


template <>
struct std::formatter<vx::ErrorFrame> {
	constexpr auto parse(std::format_parse_context &ctx) -> std::format_parse_context::iterator {
		return ctx.end() - 1;
	}


	auto format(const vx::ErrorFrame &frame, std::format_context &ctx) const noexcept
		-> std::format_context::iterator
	{
		std::string text {frame.errorCode
			| flex::pipes::to_string()
			| flex::pipes::transform([](const auto &str){return str + " "s;})
			| flex::pipes::value_or("")
		};
		text += std::format("in {} ({}:{})",
			frame.location.function_name(),
			frame.location.file_name(),
			frame.location.line()
		);
		if (frame.message)
			text += " > " + *frame.message;
		return std::ranges::copy(text, ctx.out()).out;
	};
};


template <>
struct std::formatter<vx::ErrorPayload> {
	constexpr auto parse(std::format_parse_context &ctx) -> std::format_parse_context::iterator {
		return ctx.end() - 1;
	}


	auto format(vx::ErrorPayload &payload, std::format_context &ctx) const noexcept
		-> std::format_context::iterator
	{
		std::ostringstream stream {};
		stream << "Error stack (from payload) :\n";
		for (auto &frame : payload)
			stream << "\t- " << std::format("{}", frame) << "\n";
		return std::ranges::copy(std::move(stream).str(), ctx.out()).out;
	};
};


#define makeErrorStack(...) makeErrorStack(std::source_location::current(), __VA_ARGS__)
#define addErrorToStack(...) addErrorToStack(std::source_location::current(), __VA_ARGS__)
