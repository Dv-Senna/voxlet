// ──────────────────────────  instance.hpp  ────────────────────────────
#pragma once

#include <span>
#include <string_view>
#include <utility>       // std::forward, std::move
#include <concepts>

#include "voxlet/application.hpp"
#include "voxlet/core.hpp"
#include "voxlet/error.hpp"
#include "voxlet/object.hpp"

#if __cpp_lib_expected >= 202202L
    #include <expected>
namespace vx { template <typename T> using Result = std::expected<T, vx::Error>; }
#else
    // Fallback: single‑header implementation (e.g. tl::expected) must be in include‑path.
    #include <tl/expected.hpp>
namespace vx { template <typename T> using Result = tl::expected<T, vx::Error>;   }
#endif

namespace vx {

/// Strong‑typed arguments bag for engine boot‑strapping.
struct InstanceCreateInfo {
    /// Application manifest (name, version, etc.).
    ApplicationInfos                  appInfos;
    /// Command‑line arguments exactly as passed to `main`.
    std::span<std::string_view const> args {};
};

class Instance final {
public:
    // ----------------------------------------------------------------- //
    //  Lifetime control
    // ----------------------------------------------------------------- //
    Instance() = delete;                            // Plain default construction forbidden.
    Instance(const Instance&)            = delete;  // Non‑copyable.
    auto operator=(const Instance&) 
        -> Instance&                     = delete;  // Non‑copy‑assignable.
    auto operator=(Instance&&) noexcept  
        -> Instance&                     = delete;  // Move‑assign also forbidden.

    /// Move‑construct only, so ownership can be transferred out of a factory.
    constexpr Instance(Instance&&) noexcept = default;

    /// RAII shutdown: frees all engine resources.
    VOXLET_CORE ~Instance();

    // ----------------------------------------------------------------- //
    //  Factory
    // ----------------------------------------------------------------- //
    [[nodiscard]]
    static VOXLET_CORE auto make_instance(InstanceCreateInfo createInfo) noexcept
        -> vx::Result<Instance>;

    // ----------------------------------------------------------------- //
    //  Convenience accessors
    // ----------------------------------------------------------------- //
    [[nodiscard]] constexpr bool valid() const noexcept { return m_isInitialised; }

private:
    // Private ctor used by factory once validation passes.
    explicit Instance(InstanceCreateInfo&& info) noexcept
      : m_info{ std::move(info) }, m_isInitialised{true}
    {}

    // ----------------------------------------------------------------- //
    //  Data members
    // ----------------------------------------------------------------- //
    InstanceCreateInfo         m_info;           // immutable after construction
    bool                       m_isInitialised{ false };
};

// --------------------------------------------------------------------- //
//  Compile‑time sanity
// --------------------------------------------------------------------- //
static_assert( std::movable<Instance> && !std::copyable<Instance>,
               "Instance must be movable‑only (unique owner)." );
static_assert( vx::object<Instance>, 
               "voxlet::object<Instance> concept must still evaluate to true." );

} // namespace vx
