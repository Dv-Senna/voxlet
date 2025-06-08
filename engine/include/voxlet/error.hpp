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
	 * @brief An enum representing
	 * */
	enum class ErrorCode : std::uint32_t {
		eFailure,
	};

	struct [[nodiscard]] ErrorFrame {
		std::optional<std::string> message;
		std::optional<ErrorCode> errorCode;
		std::source_location location;
	};


	class ErrorPayloadIterator {
		friend struct ErrorPayload;
		public:
			constexpr ErrorPayloadIterator() noexcept : m_frames {nullptr} {};
			inline auto operator*() const noexcept -> auto& {return m_frames->top();}
			inline auto operator->() const noexcept {return &m_frames->top();}
			inline auto operator++() const noexcept {
				if (m_frames != nullptr)
					m_frames->pop();
				return *this;
			}

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

	struct [[nodiscard]] ErrorPayload {
		std::stack<ErrorFrame> frames;
		inline auto begin() noexcept {return ErrorPayloadIterator{frames};}
		inline auto end() const noexcept {return ErrorPayloadIterator{};}
	};

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
	 * @sa vx::ErrorPayload
	 * @sa std::expected
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
