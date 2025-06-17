#include "voxlet/utils.hpp"

#include <filesystem>
#include <fstream>

#ifdef __linux__
	#include <unistd.h>
#endif


namespace vx {
	auto readBinaryFile(const std::filesystem::path &path) noexcept -> vx::Failable<std::vector<std::byte>> {
		std::ifstream file {path, std::ios::binary};
		if (!file)
			return vx::makeErrorStack(vx::ErrorCode::eInvalidPath, "Can't open binary file to read");

		const auto length {std::filesystem::file_size(path)};
		std::vector<std::byte> result (length);
		file.read(reinterpret_cast<char*> (result.data()), length);
		return result;
	}


	auto getExePath() noexcept -> vx::Failable<flex::Reference<const std::filesystem::path>> {
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


	auto getExeDirectory() noexcept -> vx::Failable<flex::Reference<const std::filesystem::path>> {
		static std::optional<std::filesystem::path> cachedExeDirectory {};
		if (cachedExeDirectory)
			return *cachedExeDirectory;
		return vx::getExePath()
			.transform([](std::filesystem::path path) {return path.remove_filename();})
			.transform([&](const auto &path) {cachedExeDirectory = path; return path;});
	}
}
