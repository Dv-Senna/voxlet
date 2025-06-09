#include "voxlet/error.hpp"
#include <cstdlib>
#include <cstring>
#include <expected>
#include <format>
#include <ranges>

#include <voxlet/application.hpp>
#include <voxlet/instance.hpp>
#include <voxlet/utils.hpp>
#include <voxlet/logger.hpp>


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

		auto onUpdate([[maybe_unused]] vx::Instance &instance, vx::Milliseconds) noexcept -> vx::Failable<void> override {
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
	return vx::makeErrorStack("Can't create app : {}", args);
	return std::make_unique<SandboxApp> ();
}

auto main(int argc, char **argv) -> int {
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

	return EXIT_SUCCESS;
}
