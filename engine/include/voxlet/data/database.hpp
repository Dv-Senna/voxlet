#pragma once

#include <flex/typeTraits.hpp>
#include <sqlite3.h>

#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"


namespace vx {
	class Instance;
};

namespace vx::data {
	class Database {
		Database(const Database&) = delete;
		auto operator=(const Database&) = delete;

		public:
			constexpr Database() noexcept = default;
			constexpr Database(Database&&) noexcept = default;
			constexpr auto operator=(Database&&) noexcept -> Database& = default;
			VOXLET_CORE ~Database();

			struct CreateInfos {
				Instance &instance;
				std::string_view name;
			};

			VOXLET_CORE static auto create(const CreateInfos &createInfos) noexcept -> vx::Failable<Database>;


		private:
			vx::BuiltFlag m_built;
			std::string m_name;
			sqlite3 *m_database;
	};
}
