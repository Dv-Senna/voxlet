#include "instance.hpp"
#include "voxlet/backend/vulkan_device.hpp"  // hypothetical subsystems …

using namespace vx;

// ------------- Destructor -------------------------------------------------- //
Instance::~Instance() {
    if (!m_isInitialised) return;           // No‑op if construction failed early.
    backend::shutdown_all();                // Tear‑down in reverse order
}

// ------------- Factory ----------------------------------------------------- //
auto Instance::make_instance(InstanceCreateInfo createInfo) noexcept
        -> vx::Result<Instance>
{
    // 1. Validate manifest
    if (createInfo.appInfos.name.empty())
        return vx::Error{vx::Errc::invalid_argument, "Application name missing"};

    // 2. Bring up low‐level subsystems
    if (auto err = backend::init_core(); err.has_value())
        return err.value();                 // early‑return with Error

    if (auto err = backend::init_windowing(createInfo.appInfos); err.has_value())
        return err.value();

    // 3. All good → hand out RAII guard
    return Instance{ std::move(createInfo) };
}
