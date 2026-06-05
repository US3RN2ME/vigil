
#ifndef VIGIL_PLATFORM_PROCESSTOKEN_HPP
#define VIGIL_PLATFORM_PROCESSTOKEN_HPP

#include <cstddef>
#include <optional>
#include <vector>

#include "Handle.hpp"

namespace vigil::platform {

class ProcessToken {
public:
  enum class InfoClass {
    User,
    Groups,
    Privileges,
    IntegrityLevel,
    Elevation,
    ElevationType
  };

  static std::optional<ProcessToken> open(Handle::NativeType process);

  ProcessToken(Handle token) noexcept;
  ~ProcessToken() noexcept = default;

  ProcessToken(const ProcessToken &) = delete;
  ProcessToken &operator=(const ProcessToken &) = delete;

  ProcessToken(ProcessToken &&) noexcept = default;
  ProcessToken &operator=(ProcessToken &&) noexcept = default;

  [[nodiscard]] std::vector<std::byte> query(InfoClass infoClass) const;

private:
  Handle handle_;
};

} // namespace vigil::platform

#endif // VIGIL_PLATFORM_PROCESSTOKEN_HPP
