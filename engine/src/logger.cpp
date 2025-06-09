#include "voxlet/logger.hpp"


namespace vx {
	auto Logger::create(CreateInfos &&createInfos) noexcept -> Logger {
		Logger logger {};
		logger.setName(createInfos.name);
		logger.setOutput(std::move(createInfos.output));
		logger.setMinimalLevel(createInfos.minimalLevel);
		return logger;
	}


	auto Logger::setOutput(Output &&output) noexcept -> void {
		m_output = std::move(output);
		if (std::holds_alternative<std::FILE*> (m_output)) {
			auto file {std::get<std::FILE*> (m_output)};
			if (file == stdout || file == stderr)
				m_colored = true;
			else
				m_colored = false;
			return;
		}

		auto file {std::get<flex::Reference<std::ostream>> (m_output)};
		if (&file == &std::cout || &file == &std::clog || &file == &std::cerr)
			m_colored = true;
		else
			m_colored = false;
	}

	Logger Logger::s_global {Logger::create(Logger::CreateInfos{
		.name = "global",
		.output = std::cout,
		.minimalLevel = LogLevel::eVerbose
	})};
}
