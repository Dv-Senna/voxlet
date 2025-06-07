#include <expected>

#include <voxlet/application.hpp>


class SandboxApp final : public vx::Application {
	public:
		SandboxApp() noexcept {
			std::println("SandboxApp created");
		}
		~SandboxApp() override {
			std::println("SandboxApp destroyed");
		}


		auto onStartup() noexcept -> vx::Result override {
			return vx::Result::eSuccess;
		}

		auto onUpdate(vx::Milliseconds) noexcept -> vx::Result override {
			return vx::Result::eSuccess;
		}

		auto onRender() noexcept -> vx::Result override {
			return vx::Result::eSuccess;
		}

		auto onShutdown() noexcept -> vx::Result override {
			return vx::Result::eSuccess;
		}
};

auto vx::createApplication([[maybe_unused]] std::span<char *const> args) noexcept
	-> vx::Failable<std::unique_ptr<vx::Application>>
{
	return std::make_unique<SandboxApp> ();
}

auto main(int argc, char **argv) -> int {
	std::span args {argv, static_cast<std::size_t> (argc)};
	vx::Failable appWithError {vx::createApplication(args)};
	if (!appWithError)
		return std::println(stderr, "ERROR : {}", appWithError.error()), EXIT_FAILURE;
	auto application {std::move(*appWithError)};

	return EXIT_SUCCESS;
}
