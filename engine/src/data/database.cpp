#include "voxlet/data/database.hpp"

#include <filesystem>

#include <sqlite3.h>

#include "voxlet/error.hpp"
#include "voxlet/instance.hpp"
#include "voxlet/logger.hpp"
#include "voxlet/utils.hpp"



static std::filesystem::path databasesDir {};

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
		databasesDir = createInfos.instance.getWorkDirectory() / "databases";
		static auto createDirectoryStatus {[](const auto &path) {
			return vx::createDirectory(path);
		} (databasesDir)};
		if (!createDirectoryStatus) [[unlikely]]
			return vx::addErrorToStack(createDirectoryStatus, "Can't create databases directory");

		Database database {};
		database.m_built = true;
		database.m_name = createInfos.name.value_or("<memory>");

		std::filesystem::path databasePath {};
		if (createInfos.name)
			databasePath = s_getFileFromName(database.m_name);
		else
			databasePath = ":memory:";
		if (sqlite3_open(databasePath.c_str(), &database.m_database) != SQLITE_OK) {
			return vx::makeErrorStack("Can't open database '{}' : {}",
				database.m_name,
				sqlite3_errmsg(database.m_database)
			);
		}

		vx::Logger::global().info("Open database '{}'", database.m_name);
		return database;
	}


	auto Database::attach(const Database &database) noexcept -> vx::Failable<void> {
		if (database.m_name == "<memory>") {
			return vx::makeErrorStack(vx::ErrorCode::eInvalidArgument,
				"Can't attach in memory database to database '{}'", m_name
			);
		}
		if (database.m_database == nullptr) {
			return vx::makeErrorStack(vx::ErrorCode::eInvalidArgument,
				"Can't attach invalid database '{}' to database '{}'", database.m_name, m_name
			);
		}
		std::filesystem::path databasePath {s_getFileFromName(database.m_name)};
		std::string query {std::format("ATTACH '{}' AS {};", databasePath.string(), database.m_name)};

		char *errorMessage {nullptr};
		int errorCode {sqlite3_exec(m_database, query.c_str(), nullptr, nullptr, &errorMessage)};
		if (errorCode == SQLITE_OK)
			return {};
		std::string ownedErrorMessage {errorMessage};
		sqlite3_free(errorMessage);
		return vx::makeErrorStack("Can't attach database '{}' to database '{}' : {}",
			database.m_name, m_name, ownedErrorMessage
		);
	}


	auto Database::s_getFileFromName(const std::string &name) -> std::filesystem::path {
		return databasesDir / (name + ".db");
	}
}
