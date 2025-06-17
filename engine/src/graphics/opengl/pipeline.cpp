#include "voxlet/graphics/opengl/pipeline.hpp"

#include <glad/glad.h>


namespace vx::graphics::opengl {
	Pipeline::~Pipeline() {
		if (!m_built)
			return;
		if (m_program == 0)
			return;
	}

	auto Pipeline::s_create(
		std::span<const vx::graphics::PipelineAttribute> attributes,
		std::span<const ShaderModulePayload> shaderModules
	) noexcept -> vx::Failable<Pipeline> {
		Pipeline pipeline {};
		pipeline.m_built = true;
		pipeline.m_shaderModulePayloads = std::vector(shaderModules.begin(), shaderModules.end());
		pipeline.m_attributes = std::vector(attributes.begin(), attributes.end());

		pipeline.m_program = glCreateProgram();
		for (const auto &shaderPayload : pipeline.m_shaderModulePayloads)
			glAttachShader(pipeline.m_program, shaderPayload.shaderModule);

		glLinkProgram(pipeline.m_program);

		for (const auto &shaderPayload : pipeline.m_shaderModulePayloads)
			glDetachShader(pipeline.m_program, shaderPayload.shaderModule);

		GLint linkStatus {};
		glGetProgramiv(pipeline.m_program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus)
			return pipeline;

		GLint logLength {};
		glGetProgramiv(pipeline.m_program, GL_INFO_LOG_LENGTH, &logLength);
		std::string log {};
		log.resize(logLength);
		glGetProgramInfoLog(pipeline.m_program, logLength, &logLength, log.data());
		return vx::makeErrorStack("Can't link program : {}", log);
	}
}
