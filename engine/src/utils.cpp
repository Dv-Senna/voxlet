#include "voxlet/utils.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>

#ifdef __linux__
	#include <unistd.h>
#endif


namespace vx {
	auto readBinaryFile(const std::filesystem::path &path) noexcept -> vx::Failable<std::vector<std::byte>> {
		std::ifstream file {path, std::ios::binary};
		if (!file) {
			return vx::makeErrorStack(vx::ErrorCode::eInvalidPath,
				"Can't open binary file '{}' to read",
				path.string()
			);
		}

		const auto length {std::filesystem::file_size(path)};
		std::vector<std::byte> result (length);
		file.read(reinterpret_cast<char*> (result.data()), length);
		return result;
	}


	auto getExePath() noexcept -> vx::Failable<std::filesystem::path> {
		static std::optional<std::filesystem::path> cachedExePath {};
		if (cachedExePath)
			return *cachedExePath;

	#ifdef __linux__
		constexpr std::size_t BUFFER_SIZE {512};
		std::string exePath {};
		exePath.resize(BUFFER_SIZE);

		const ssize_t written {readlink("/proc/self/exe", exePath.data(), BUFFER_SIZE)};
		if (written < 0)
			return vx::makeErrorStack("Can't get exe path");
		exePath.resize(written);
		cachedExePath = std::filesystem::path{exePath};
		return *cachedExePath;
	#else
		// ...
	#endif
	}


	auto getExeDirectory() noexcept -> vx::Failable<std::filesystem::path> {
		static std::optional<std::filesystem::path> cachedExeDirectory {};
		if (cachedExeDirectory)
			return *cachedExeDirectory;
		auto exePath {vx::getExePath()};
		if (!exePath)
			return std::unexpected{exePath.error()};
		cachedExeDirectory = exePath->remove_filename();
		return *cachedExeDirectory;
	}


	auto createDirectory(const std::filesystem::path &path) noexcept -> vx::Failable<bool> {
		std::error_code errorCode {};
		bool res {std::filesystem::create_directory(path, errorCode)};
		if (errorCode)
			return vx::makeErrorStack("Can't create directory : {}", errorCode.value());
		return res;
	}


	auto createDirectoryRecursively(const std::filesystem::path &path) noexcept -> vx::Failable<bool> {
		std::error_code errorCode {};
		bool res {std::filesystem::create_directories(path, errorCode)};
		if (errorCode)
			return vx::makeErrorStack("Can't create directory recursively : {}", errorCode.value());
		return res;
	}
}
