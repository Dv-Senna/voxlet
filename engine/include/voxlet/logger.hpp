#pragma once

#include <cstdlib>
#include <iostream>
#include <variant>

#include <flex/pipes/toString.hpp>
#include <flex/pipes/valueOr.hpp>
#include <flex/reference.hpp>

#include "voxlet/object.hpp"


namespace vx {
	enum class LogLevel {
		eVerbose,
		eInfo,
		eWarning,
		eError,
		eFatal
	};

	class Logger final : vx::Object {
		using Output = std::variant<flex::Reference<std::ostream>, std::FILE*>;
		public:
			constexpr Logger() noexcept :
				m_name {"<no-name>"},
				m_output {std::cout},
				m_minimalLevel {vx::LogLevel::eInfo},
				m_colored {true}
			{}
			inline Logger(Logger &&logger) noexcept :
				m_name {std::move(logger.m_name)},
				m_output {std::move(logger.m_output)},
				m_minimalLevel {logger.m_minimalLevel},
				m_colored {logger.m_colored}
			{}

			struct CreateInfos {
				std::string_view name;
				Output output;
				vx::LogLevel minimalLevel {vx::LogLevel::eInfo};
			};

			static auto create(CreateInfos &&createInfos) noexcept -> Logger;
			inline static auto global() noexcept -> Logger& {return s_global;}

			auto setOutput(Output &&output) noexcept -> void;
			inline auto setName(std::string_view name) noexcept -> void {m_name = name;}
			inline auto setMinimalLevel(vx::LogLevel minimalLevel) noexcept -> void {m_minimalLevel = minimalLevel;}

			template <typename ...Args>
			inline auto log(LogLevel level, std::format_string<Args...> format, Args &&...args) noexcept -> void {
				if (level < m_minimalLevel)
					return;
				if (std::holds_alternative<std::FILE*> (m_output))
					this->m_log<std::FILE*, Args...> (level, format, std::forward<Args> (args)...);
				else
					this->m_log<flex::Reference<std::ostream>, Args...> (level, format, std::forward<Args> (args)...);
			}

			template <typename ...Args>
			inline auto verbose(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eVerbose, format, std::forward<Args> (args)...);
			}
			template <typename ...Args>
			inline auto info(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eInfo, format, std::forward<Args> (args)...);
			}
			template <typename ...Args>
			inline auto warning(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eWarning, format, std::forward<Args> (args)...);
			}
			template <typename ...Args>
			inline auto error(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eError, format, std::forward<Args> (args)...);
			}
			template <typename ...Args>
			inline auto fatal(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eFatal, format, std::forward<Args> (args)...);
			}

		private:
			template <typename T, typename ...Args>
			auto m_log(LogLevel level, std::format_string<Args...> format, Args &&...args) noexcept -> void;

			std::string m_name;
			Output m_output;
			vx::LogLevel m_minimalLevel;
			bool m_colored;

			static Logger s_global;
	};

	static_assert(vx::object<Logger>);


	template <typename T, typename ...Args>
	auto Logger::m_log(LogLevel level, std::format_string<Args...> format, Args &&...args) noexcept -> void {
		const auto levelString {level
			| flex::pipes::to_string()
			| flex::pipes::value_or("<invalid>")
		};

		const std::string text {std::format(format, std::forward<Args> (args)...)};
		std::string_view colorStart {""};
		std::string_view colorEnd {""};
		if (m_colored) {
			colorEnd = "\033[m";
			switch (level) {
				using enum vx::LogLevel;
				case eVerbose: colorStart = "\033[90m"; break;
				case eInfo: colorStart = "\033[34m"; break;
				case eWarning: colorStart = "\033[33m"; break;
				case eError: colorStart = "\033[31m"; break;
				case eFatal: colorStart = "\033[91m"; break;
				default: colorStart = "\033[35m";
			}
		}
		std::println(std::get<T> (m_output), "{}[{}] {} > {}{}", colorStart, m_name, levelString, text, colorEnd);
	}
}
