
#ifndef VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP
#define VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP

#include <cstdint>
#include <optional>
#include <string>

#include <vigil/ProcessInfo.hpp>

namespace vigil::platform::windows {
   std::optional<ProcessInfo> readProcessInfo(uint32_t pid);
   std::optional<std::string> readProcessName(uint32_t pid);
} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP
