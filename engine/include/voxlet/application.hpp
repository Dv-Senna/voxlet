#pragma once
/** @file */

#include <memory>
#include <span>
#include <string_view>

#include "voxlet/error.hpp"
#include "voxlet/units.hpp"
#include "voxlet/utils.hpp"


namespace vx {
	class Instance;

	/**
	 * @brief A struct representing informations about your application
	 * */
	struct ApplicationInfos {
		/**
		 * @brief The name of your application. Typically used by GPU driver to improve performance
		 * */
		std::string_view name;
		/**
		 * @brief The version of your application. Typically used by GPU driver to improve performance
		 * */
		vx::Version version;
	};

	/**
	 * @brief A polymorphic interface used to implement your own Voxlet's application
	 * @note To create your own application, please define `vx::createApplication()`
	 * @sa vx::createApplication()
	 * */
	class Application {
		public:
			/**
			 * @brief The constructor containing the informations about the application. Those infos
			 *        are usually known at compile time
			 * @param infos The application's informations
			 * @sa vx::ApplicationInfos
			 * @sa vx::createApplication()
			 * */
			constexpr Application(ApplicationInfos &&infos) noexcept : m_infos {std::move(infos)} {}
			virtual constexpr ~Application() = default;

			/**
			 * @brief A method run during the startup phase of the application
			 * @param instance The instance of Voxlet
			 * @return Whether the method succed or not, with error informations in the latter
			 *
			 * This method allow you to create resources with Voxlet, as it is the first time you
			 * have access to instance of the engine. This method is called only once by the engine,
			 * right after creating its instance and loading all its subsystems. This is, in a way,
			 * the constructor of your application. The real constructor should only be used for
			 * configurations and stuff that can't go elsewhere.
			 *
			 * @sa vx::Instance
			 * @sa vx::Application::Application
			 * @sa vx::Application::onShutdown
			 * @sa vx::Failable
			 * */
			virtual auto onStartup(Instance &instance) noexcept -> vx::Failable<void> = 0;

			/**
			 * @brief A method run during the update phase of the application, before Voxlet's
			 *        subsystem updates
			 * @param instance The instance of Voxlet
			 * @param dt The duration of the last frame. Used to make updating framerate-independant
			 * @return Whether the method succed or not, with error informations in the latter
			 *
			 * This method allow you to update stuff such as position of the player. It is called right
			 * after event processing, but before Voxlet's subsystem update. If your updates need to be
			 * after Voxlet's subsystem update, look for `vx::Application::onAfterUpdates`.
			 *
			 * @sa vx::Instance
			 * @sa vx::Application::onAfterUpdates
			 * @sa vx::Failable
			 * */
			virtual inline auto onBeforeUpdates(
				[[maybe_unused]] Instance &instance,
				[[maybe_unused]] vx::Milliseconds dt
			) noexcept -> vx::Failable<void> {return {};}

			/**
			 * @brief A method run during the update phase of the application, after Voxlet's
			 *        subsystem updates
			 * @param instance The instance of Voxlet
			 * @param dt The duration of the last frame. Used to make updating framerate-independant
			 * @return Whether the method succed or not, with error informations in the latter
			 *
			 * This method allow you to update stuff such as position of the player. It is called right
			 * after Voxlet's subsystem update. If your updates need to be before Voxlet's subsystem update,
			 * look for `vx::Application::onBeforeUpdates`.
			 *
			 * @sa vx::Instance
			 * @sa vx::Application::onBeforeUpdates
			 * @sa vx::Failable
			 * */
			virtual inline auto onAfterUpdates(
				[[maybe_unused]] Instance &instance,
				[[maybe_unused]] vx::Milliseconds dt
			) noexcept -> vx::Failable<void> {return {};}

			/**
			 * @brief A method run during the rendering phase of the application, after Voxlet's
			 *        renderer rendering phase
			 * @param instance The instance of Voxlet
			 * @return Whether the method succed or not, with error informations in the latter
			 *
			 * This method allow you to do post-processing stuff and to do old and dirty rendering
			 * if Voxlet's doesn't have something you want.
			 *
			 * @sa vx::Instance
			 * @sa vx::Failable
			 * */
			virtual auto onRender(Instance &instance) noexcept -> vx::Failable<void> = 0;

			/**
			 * @brief A method run during the shutdown phase of the application
			 * @param instance The instance of Voxlet
			 * @return Whether the method succed or not, with error informations in the latter
			 *
			 * This method allow you to destroy ressources created in `vx::Application::onStartup` with
			 * Voxlet, as it is the last time you have access to the instance of the engine. This
			 * method is called only once by the engine, right before unloading all its subsystems and
			 * destroying its instance. This is, in a way, the destructor of your application. The real
			 * destructor should only be used for configurations and stuff that can't go elsewhere.
			 *
			 * @sa vx::Instance
			 * @sa vx::Application::onStartup
			 * @sa vx::Failable
			 * */
			virtual auto onShutdown(Instance &instance) noexcept -> vx::Failable<void> = 0;

			/**
			 * @brief Getter
			 * @return The infos of the application
			 * */
			constexpr auto getInfos() const noexcept -> const ApplicationInfos& {return m_infos;}

		private:
			ApplicationInfos m_infos;
	};

	/**
	 * @brief Create your application.
	 * @param args The command line arguments used to launch your application
	 * @return A `std::unique_ptr` to your application, or error informations in case of failure
	 *
	 * This function **must** be defined somewhere in your application, as this is more or less your entry
	 * point, as Voxlet handled the main function for you. Here, you create an instance of your application,
	 * which **must** be a concrete implementation of `vx::Application`. This function must really be understood
	 * as a configuration / selection step, and **must not** start to do stuff like resource creation.
	 *
	 * @note As this function will usually be defined only once, it is recommended to define `VOXLET_MAIN_FILE`
	 *       before including Voxlet in the same source file as this to incorporate the main function into
	 *       your program.
	 *
	 * @code{.cpp}
	 * // we suppose `MyApp` is the type of your application
	 * auto vx::createApplication([[maybe_unused]] std::span<const std::string_view> args) noexcept
	 *     -> vx::Failable<std::unique_ptr<vx::Application>>
	 * {
	 *     return std::make_unique<MyApp> ();
	 * }
	 * @endcode
	 *
	 * @sa vx::Application
	 * @sa vx::Application::Application
	 * @sa vx::Failable
	 * */
	auto createApplication(std::span<const std::string_view> args) noexcept
		-> vx::Failable<std::unique_ptr<Application>>;
}


#ifdef VOXLET_MAIN_FILE
	auto main(int argc, char **argv) -> int {
		vx::Failable appWithError {vx::createApplication(std::span{argv, argc})};
		if (!appWithError)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
#endif
