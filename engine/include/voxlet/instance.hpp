#pragma once

#include <span>
#include <string_view>

#include "voxlet/application.hpp"
#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx {
	/**
	 * @brief Object that manage the whole life of the engine. Also the main
	 *        interaction point with Voxlet for the application
	 * */
	class Instance final {
		Instance(const Instance&) = delete;
		auto operator=(const Instance&) -> Instance& = delete;
		auto operator=(Instance&&) -> Instance& = delete;

		public:
			VOXLET_CORE ~Instance();
			/**
			 * @brief Move constructor
			 * */
			constexpr Instance(Instance&&) noexcept = default;

			/**
			 * @brief Informations to create the instance
			 * */
			struct CreateInfos {
				/**
				 * @brief The informations of the application
				 * @sa vx::ApplicationInfos
				 * @sa vx::Application
				 * */
				const vx::ApplicationInfos &appInfos;
				/**
				 * @brief Command line arguments used when launching the
				 *        application executable
				 * */
				const std::span<const std::string_view> args;
			};

			/**
			 * @brief Create the instance of Voxlet
			 * @param createInfos The informations needed to create the instance
			 * @return The created instance, or an error in case of failure
			 * */
			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept -> vx::Failable<Instance>;

		private:
			constexpr Instance() noexcept = default;

			vx::BuiltFlag m_built;
	};

	static_assert(vx::object<Instance>);
}
