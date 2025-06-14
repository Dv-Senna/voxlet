#include "voxlet/error.hpp"
#include "voxlet/graphics/buffer.hpp"
#include <SDL3/SDL_video.h>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <format>
#include <ranges>

#include <SDL3/SDL.h>

#include <voxlet/application.hpp>
#include <voxlet/instance.hpp>
#include <voxlet/utils.hpp>
#include <voxlet/logger.hpp>

#include <voxlet/graphics/opengl/buffer.hpp>


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
	if (!appWithError)
		return vx::Logger::global().fatal("Can't create app : {}", appWithError.error()), EXIT_FAILURE;
	auto application {std::move(*appWithError)};

	vx::Instance::CreateInfos instanceCreateInfos {
		.appInfos = application->getInfos(),
		.args = args
	};
	vx::Failable instanceWithError {vx::Instance::create(instanceCreateInfos)};
	if (!instanceWithError)
		return vx::Logger::global().fatal("Can't create instance : {}", instanceWithError.error()), EXIT_FAILURE;
	auto instance {std::move(*instanceWithError)};


	vx::graphics::BufferDescriptor bufferDescriptor {
		.size = 64_MiB,
		.access = {},//vx::graphics::BufferAccess::eCpuReadable,
		.content = std::nullopt
	};
	vx::Failable bufferWithError {vx::graphics::opengl::Buffer<
		vx::graphics::BufferType::eVertex
	>::create(bufferDescriptor)};
	if (!bufferWithError)
		return vx::Logger::global().fatal("Can't create buffer : {}", instanceWithError.error()), EXIT_FAILURE;
	auto buffer {std::move(*bufferWithError)};


	bool running {true};
	while (running) {
		SDL_Event event {};
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			else if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
				running = false;
		}

		if (!SDL_GL_SwapWindow(instance.getWindow().getWindow()))
			return vx::Logger::global().fatal("Can't swap window : {}", SDL_GetError()), EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
