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


namespace vx {
	enum class Result : std::int32_t {
		eSuccess = 0,
		eFailure = -1
	};

	constexpr auto operator!(Result res) noexcept -> bool {
		return std::to_underlying(res) >= std::underlying_type_t<Result> (0);
	}


	struct ErrorFrame {
		std::optional<std::string> message;
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

	struct ErrorPayload {
		std::stack<ErrorFrame> frames;
		inline auto begin() noexcept {return ErrorPayloadIterator{frames};}
		inline auto end() const noexcept {return ErrorPayloadIterator{};}
	};

	template <typename ...Args>
	constexpr auto makeErrorStack(
		std::source_location &&location,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		ErrorPayload payload {};
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}

	template <typename ...Args>
	constexpr auto addErrorToStack(
		std::source_location &&location,
		ErrorPayload &payload,
		std::format_string<Args...> format,
		Args &&...args
	) noexcept {
		payload.frames.push(ErrorFrame{
			.message = std::format(format, std::forward<Args> (args)...),
			.location = std::move(location)
		});
		return std::unexpected(std::move(payload));
	}


	template <typename T>
	using Failable = std::expected<T, ErrorPayload>;
}


template <>
struct std::formatter<vx::ErrorFrame> {
	constexpr auto parse(std::format_parse_context &ctx) -> std::format_parse_context::iterator {
		return ctx.end() - 1;
	}


	auto format(const vx::ErrorFrame &frame, std::format_context &ctx) const noexcept
		-> std::format_context::iterator
	{
		std::string text {std::format("in {} ({}:{})",
			frame.location.function_name(),
			frame.location.file_name(),
			frame.location.line()
		)};
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
