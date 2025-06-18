#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>


namespace vx::graphics {
	enum class ShaderModuleStage : std::uint8_t {
		eVertex,
		eGeometry,
		eFragment,
		eTesselationControl,
		eTesselationEvaluation,
		eCompute
	};

	struct ShaderModuleDescriptor {
		std::filesystem::path source;
		std::string_view entryPoint;
	};


	template <ShaderModuleStage stage>
	class ShaderModule {
		using This = ShaderModule<stage>;
		ShaderModule(const This&) = delete;
		auto operator=(const This&) -> This& = delete;

		public:
			constexpr ShaderModule() noexcept = default;
			constexpr ShaderModule(This&&) noexcept = default;
			constexpr auto operator=(This&&) noexcept -> This& = default;
			virtual constexpr ~ShaderModule() = default;

			virtual auto getSourcePath() const noexcept -> const std::filesystem::path& = 0;
			virtual auto getEntryPoint() const noexcept -> std::string_view = 0;
	};
}
