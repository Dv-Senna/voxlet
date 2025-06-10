#pragma once

#include <cstdlib>
#include <iostream>
#include <variant>

#include <flex/pipes/toString.hpp>
#include <flex/pipes/valueOr.hpp>
#include <flex/reference.hpp>

#include "voxlet/core.hpp"
#include "voxlet/object.hpp"


namespace vx {
	/**
	 * @brief The level of a log
	 * @sa vx::Logger
	 * @sa vx::Logger::log
	 * */
	enum class LogLevel {
		/**
		 * @brief The least important and most spamming logs. Usually some
		 *        tracking stuffs
		 * */
		eVerbose,
		/**
		 * @brief Informations about the engine that has a certain importance,
		 *        but doesn't have any other semantic meaning
		 * */
		eInfo,
		/**
		 * @brief Warnings. Usually some suboptimal usage of the API, deprecated
		 *        stuffs or very minor failure
		 * */
		eWarning,
		/**
		 * @brief Indicate that an error happened. The error is usually
		 *        recoverable-from or is a past failure that ultimately produced
		 *        a fatal failure
		 * */
		eError,
		/**
		 * @brief Indicate a catastrophic failure that produce a crash of the
		 *        engine / application
		 * */
		eFatal
	};

	/**
	 * @brief An object representing a logger
	 *
	 * A logger is in Voxlet an object you can create at any point in time and
	 * use to log stuff to some output, it being either an `std::ostream` or
	 * a good old `FILE*`.
	 *
	 * @note The logger doesn't own the output.
	 * @warning The logger **are not** thread-safe, except if your output is
	 *          thread-safe and you don't use the logger's setters
	 * @sa vx::Logger::log()
	 * */
	class Logger final : vx::Object {
		using Output = std::variant<flex::Reference<std::ostream>, std::FILE*>;
		public:
			constexpr Logger() noexcept :
				m_name {"<no-name>"},
				m_output {std::cout},
				m_minimalLevel {vx::LogLevel::eInfo},
				m_colored {true}
			{}
			/**
			 * @brief Move constructor
			 * */
			inline Logger(Logger &&logger) noexcept :
				m_name {std::move(logger.m_name)},
				m_output {std::move(logger.m_output)},
				m_minimalLevel {logger.m_minimalLevel},
				m_colored {logger.m_colored}
			{}

			/**
			 * @brief Informations to create a logger
			 * @sa vx::Logger::create()
			 * */
			struct CreateInfos {
				/**
				 * @brief The name of the logger.
				 * @note This string will be copied, so you can destroy your
				 *       string right after creating the logger
				 * */
				std::string_view name;
				/**
				 * @brief The output of the logger. Either an `std::ostream&`
				 *        or a `FILE*`
				 * @warning The output **must** live at least as long as the
				 *          logger, as it does not own it
				 * */
				Output output;
				/**
				 * @brief The minimal level of the log. Everything under this
				 *        will not be logged
				 * */
				vx::LogLevel minimalLevel {vx::LogLevel::eInfo};
			};

			/**
			 * @brief Create an instance of `vx::Logger`
			 * @param createInfos The informations needed to create the logger
			 * @return The newly-created instance of `vx::Logger` logger
			 * @sa vx::Logger::CreateInfos
			 * */
			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept -> Logger;
			/**
			 * @brief Get access to the global logger
			 * @return A reference to the global logger of Voxlet
			 * */
			inline static auto global() noexcept -> Logger& {return s_global;}

			/**
			 * @brief Set the output of the logger
			 * @param output The new output of the logger
			 * @sa vx::Logger::CreateInfos::output
			 * */
			VOXLET_CORE auto setOutput(const Output &output) noexcept -> void;
			/**
			 * @brief Set the name of the logger
			 * @param name The new name of the logger
			 * @sa vx::Logger::CreateInfos::name
			 * */
			inline auto setName(std::string_view name) noexcept -> void {m_name = name;}
			/**
			 * @brief Set the minimal log level of the logger
			 * @param minimalLevel The new minimal log level of the logger
			 * @sa vx::Logger::CreateInfos::minimalLevel
			 * */
			inline auto setMinimalLevel(vx::LogLevel minimalLevel) noexcept -> void {m_minimalLevel = minimalLevel;}

			/**
			 * @brief Log a message to the output of the logger, with a given level
			 * @param  level  The level of the log
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::verbose()
			 * @sa vx::Logger::info()
			 * @sa vx::Logger::warning()
			 * @sa vx::Logger::error()
			 * @sa vx::Logger::fatal()
			 * */
			template <typename ...Args>
			inline auto log(LogLevel level, std::format_string<Args...> format, Args &&...args) noexcept -> void {
				if (level < m_minimalLevel)
					return;
				if (std::holds_alternative<std::FILE*> (m_output))
					this->m_log<std::FILE*, Args...> (level, format, std::forward<Args> (args)...);
				else
					this->m_log<flex::Reference<std::ostream>, Args...> (level, format, std::forward<Args> (args)...);
			}

			/**
			 * @brief Log a verbose message to the output of the logger
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::log()
			 * */
			template <typename ...Args>
			inline auto verbose(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eVerbose, format, std::forward<Args> (args)...);
			}
			/**
			 * @brief Log an info message to the output of the logger
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::log()
			 * */
			template <typename ...Args>
			inline auto info(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eInfo, format, std::forward<Args> (args)...);
			}
			/**
			 * @brief Log a warning message to the output of the logger
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::log()
			 * */
			template <typename ...Args>
			inline auto warning(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eWarning, format, std::forward<Args> (args)...);
			}
			/**
			 * @brief Log an error message to the output of the logger
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::log()
			 * */
			template <typename ...Args>
			inline auto error(std::format_string<Args...> format, Args &&...args) noexcept -> void {
				return this->log(LogLevel::eError, format, std::forward<Args> (args)...);
			}
			/**
			 * @brief Log a fatal message to the output of the logger
			 * @param  format The format-string of the text
			 * @param  args   The arguments to use for the formatting of the text
			 * @tparam Args   The type of the arguments to use for formatting
			 * @sa vx::Logger::log()
			 * */
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

			VOXLET_CORE static Logger s_global;
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
