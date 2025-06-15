#include "voxlet/graphics/opengl/shaderModule.hpp"

#include <flex/reflection/enums.hpp>

#include "voxlet/enumDispatcher.hpp"
#include "voxlet/graphics/opengl/function.hpp"
#include "voxlet/graphics/shaderModule.hpp"
#include "voxlet/utils.hpp"


namespace vx::graphics::opengl {
	ShaderModuleBase::~ShaderModuleBase() {
		if (!m_built)
			return;
	}


	auto ShaderModuleBase::create(const CreateInfos &createInfos) noexcept
		-> vx::Failable<ShaderModuleBase>
	{
		static const vx::EnumDispatcher shaderModuleStageDispatcher {
			std::pair{vx::graphics::ShaderModuleStage::eVertex,                GL_VERTEX_SHADER},
			std::pair{vx::graphics::ShaderModuleStage::eGeometry,              GL_GEOMETRY_SHADER},
			std::pair{vx::graphics::ShaderModuleStage::eFragment,              GL_FRAGMENT_SHADER},
			std::pair{vx::graphics::ShaderModuleStage::eTesselationControl,    GL_TESS_CONTROL_SHADER},
			std::pair{vx::graphics::ShaderModuleStage::eTesselationEvaluation, GL_TESS_EVALUATION_SHADER},
			std::pair{vx::graphics::ShaderModuleStage::eCompute,               GL_COMPUTE_SHADER},
		};

		std::optional stageWithError {shaderModuleStageDispatcher(createInfos.stage)};
		if (!stageWithError) {
			return vx::makeErrorStack("Invalid shader module stage : {}",
				flex::toString(createInfos.stage).value_or("<invalid>")
			);
		}

		vx::Failable spirvWithError {vx::readBinaryFile(createInfos.source)};
		if (!spirvWithError)
			return vx::addErrorToStack(spirvWithError, "Can't read spir-v file for ShaderModule");

		ShaderModuleBase shaderModule {};
		shaderModule.m_built = true;
		shaderModule.m_sourcePath = createInfos.source;
		shaderModule.m_entryPoint = createInfos.entryPoint;
		shaderModule.m_stage = createInfos.stage;

		shaderModule.m_shaderModule = glCreateShader(*stageWithError);
		vx::Failable shaderBinaryError {vx::graphics::opengl::call(glShaderBinary,
			1, &shaderModule.m_shaderModule,
			GL_SHADER_BINARY_FORMAT_SPIR_V,
			spirvWithError->data(), spirvWithError->size()
		)};
		if (!shaderBinaryError)
			return vx::addErrorToStack(shaderBinaryError, "Can't send shader spir-v binary");

		vx::Failable specializeError {vx::graphics::opengl::call(glSpecializeShader,
			shaderModule.m_shaderModule,
			shaderModule.m_entryPoint.c_str(),
			0, nullptr, nullptr
		)};
		if (!specializeError)
			return vx::addErrorToStack(specializeError, "Call to specialize shader failed for some reasons");

		GLint compilationStatus {};
		glGetShaderiv(shaderModule.m_shaderModule, GL_COMPILE_STATUS, &compilationStatus);
		if (compilationStatus)
			return shaderModule;

		GLint logLength {};
		glGetShaderiv(shaderModule.m_shaderModule, GL_INFO_LOG_LENGTH, &logLength);
		std::string log {};
		log.resize(logLength);
		glGetShaderInfoLog(shaderModule.m_shaderModule, logLength, &logLength, log.data());
		return vx::makeErrorStack("Can't specialize shader : {}", log);
	}

}
