#include "voxlet/data/database.hpp"

#include <filesystem>

#include <sqlite3.h>

#include "voxlet/error.hpp"
#include "voxlet/instance.hpp"
#include "voxlet/logger.hpp"
#include "voxlet/utils.hpp"


namespace vx::data {
	Database::~Database() {
		if (!m_built)
			return;
		if (m_database == nullptr)
			return;
		sqlite3_close(m_database);
		vx::Logger::global().info("Close database '{}'", m_name);
	}


	auto Database::create(const CreateInfos &createInfos) noexcept -> vx::Failable<Database> {
		static std::filesystem::path databasesDir {createInfos.instance.getWorkDirectory() / "databases"};
		static auto createDirectoryStatus {[](const auto &path) {
			return vx::createDirectory(path);
		} (databasesDir)};
		if (!createDirectoryStatus) [[unlikely]]
			return vx::addErrorToStack(createDirectoryStatus, "Can't create databases directory");

		Database database {};
		database.m_built = true;
		database.m_name = createInfos.name;

		std::filesystem::path databasePath {databasesDir / (database.m_name + ".db")};
		if (sqlite3_open(databasePath.c_str(), &database.m_database) != SQLITE_OK) {
			return vx::makeErrorStack("Can't open database '{}' : {}",
				database.m_name,
				sqlite3_errmsg(database.m_database)
			);
		}

		vx::Logger::global().info("Open database '{}'", database.m_name);
		return database;
	}
}
