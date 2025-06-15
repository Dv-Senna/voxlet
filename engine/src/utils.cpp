#include "voxlet/utils.hpp"

#include <filesystem>
#include <fstream>


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
}
