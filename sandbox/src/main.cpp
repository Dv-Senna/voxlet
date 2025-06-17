#include "voxlet/error.hpp"
#include "voxlet/graphics/buffer.hpp"
#include "voxlet/graphics/shaderModule.hpp"
#include <SDL3/SDL_video.h>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <ranges>

#include <SDL3/SDL.h>

#include <voxlet/application.hpp>
#include <voxlet/instance.hpp>
#include <voxlet/utils.hpp>
#include <voxlet/logger.hpp>

#include <voxlet/graphics/opengl/buffer.hpp>
#include <voxlet/graphics/opengl/pipeline.hpp>


class SandboxApp final : public vx::Application {
	public:
		SandboxApp() noexcept : vx::Application{vx::ApplicationInfos{
			.name = "SandboxApp",
			.version = vx::voxletVersion
		}} {
			vx::Logger::global().verbose("SandboxApp created");
		}
		~SandboxApp() override {
			vx::Logger::global().verbose("SandboxApp destroyed");
		}


		auto onStartup([[maybe_unused]] vx::Instance &instance) noexcept -> vx::Failable<void> override {
			return {};
		}

		auto onBeforeUpdates([[maybe_unused]] vx::Instance &instance, vx::Milliseconds) noexcept -> vx::Failable<void> override {
			return {};
		}

		auto onRender([[maybe_unused]] vx::Instance &instance) noexcept -> vx::Failable<void> override {
			return {};
		}

		auto onShutdown([[maybe_unused]] vx::Instance &instance) noexcept -> vx::Failable<void> override {
			return {};
		}
};

auto vx::createApplication([[maybe_unused]] std::span<const std::string_view> args) noexcept
	-> vx::Failable<std::unique_ptr<vx::Application>>
{ 
	return std::make_unique<SandboxApp> ();
}

auto main(int argc, char **argv) -> int {
	using namespace vx::units::bytes::literals;
	auto args {std::span{argv, static_cast<std::size_t> (argc)}
		| std::views::transform([](const char *arg) {return std::string_view{arg, std::strlen(arg)};})
		| std::ranges::to<std::vector> ()
	};
	vx::Failable appWithError {vx::createApplication(args)};
	if (!appWithError) [[unlikely]]
		return vx::Logger::global().fatal("Can't create app : {}", appWithError.error()), EXIT_FAILURE;
	auto application {std::move(*appWithError)};

	vx::Instance::CreateInfos instanceCreateInfos {
		.appInfos = application->getInfos(),
		.args = args
	};
	vx::Failable instanceWithError {vx::Instance::create(instanceCreateInfos)};
	if (!instanceWithError) [[unlikely]]
		return vx::Logger::global().fatal("Can't create instance : {}", instanceWithError.error()), EXIT_FAILURE;
	auto instance {std::move(*instanceWithError)};


	std::array initialBufferData {
		-0.5f, -0.5f,  1.f, 0.f, 0.f,
		0.5f, -0.5f,   0.f, 1.f, 0.f,
		0.f, 0.5f,     0.f, 0.f, 1.f
	};

	vx::graphics::BufferDescriptor bufferDescriptor {
		.size = 64_MiB,
		.access = vx::graphics::BufferAccess::eCpuWritable | vx::graphics::BufferAccess::eCpuReadable,
		.content = std::nullopt
	};
	vx::Failable bufferWithError {vx::graphics::opengl::Buffer<
		vx::graphics::BufferType::eVertex
	>::create(bufferDescriptor)};
	if (!bufferWithError) [[unlikely]]
		return vx::Logger::global().fatal("Can't create buffer : {}", bufferWithError.error()), EXIT_FAILURE;
	auto buffer {std::move(*bufferWithError)};

	vx::Failable bufferWriteError {buffer.write(0_B, std::as_bytes(std::span{initialBufferData}))};
	if (!bufferWriteError)
		return vx::Logger::global().fatal("Can't write data to buffer : {}", bufferWriteError.error()), EXIT_FAILURE;


	vx::Failable exeDirectoryWithError {vx::getExeDirectory()};
	if (!exeDirectoryWithError)
		return vx::Logger::global().fatal("Can't get exe directory : {}", exeDirectoryWithError.error()), EXIT_FAILURE;
	const std::filesystem::path exeDirectory {*exeDirectoryWithError};

	vx::graphics::ShaderModuleDescriptor vertexShaderModuleDescriptor {
		.source = exeDirectory / "shaders/triangle.vert",
		.entryPoint = "main"
	};
	vx::Failable vertexShaderWithError {vx::graphics::opengl::ShaderModule<
		vx::graphics::ShaderModuleStage::eVertex
	>::create(vertexShaderModuleDescriptor)};
	if (!vertexShaderWithError)
		return vx::Logger::global().fatal("Can't create vertex shader : {}", vertexShaderWithError.error()), EXIT_FAILURE;
	auto vertexShader {std::move(*vertexShaderWithError)};

	vx::graphics::ShaderModuleDescriptor fragmentModuleDescriptor {
		.source = exeDirectory / "shaders/triangle.frag",
		.entryPoint = "main"
	};
	vx::Failable fragmentShaderWithError {vx::graphics::opengl::ShaderModule<
		vx::graphics::ShaderModuleStage::eFragment
	>::create(fragmentModuleDescriptor)};
	if (!fragmentShaderWithError)
		return vx::Logger::global().fatal("Can't create vertex shader : {}", fragmentShaderWithError.error()), EXIT_FAILURE;
	auto fragmentShader {std::move(*fragmentShaderWithError)};

	vx::graphics::PipelineDescriptor pipelineDescriptor {
		.attributes = {},
		.shaderModules = std::tie(
			static_cast<vx::graphics::ShaderModule<vx::graphics::ShaderModuleStage::eVertex>&> (vertexShader),
			static_cast<vx::graphics::ShaderModule<vx::graphics::ShaderModuleStage::eFragment>&> (fragmentShader)
		)
	};
	vx::Failable pipelineWithError {vx::graphics::opengl::Pipeline::create(pipelineDescriptor)};
	if (!pipelineWithError)
		return vx::Logger::global().fatal("Can't create pipeline : {}", pipelineWithError.error()), EXIT_FAILURE;
	auto pipeline {std::move(*pipelineWithError)};


	bool running {true};
	while (running) {
		SDL_Event event {};
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
				running = false;
		}

		if (!SDL_GL_SwapWindow(instance.getWindow().getWindow())) [[unlikely]]
			return vx::Logger::global().fatal("Can't swap window : {}", SDL_GetError()), EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
