
#ifndef VIGIL_PLATFORM_LINUX_PROCFS_HPP
#define VIGIL_PLATFORM_LINUX_PROCFS_HPP

#include <optional>

#include "vigil/ProcessInfo.hpp"

namespace vigil::platform::linux {
   std::optional<ProcessInfo> readProcessInfo(int pid);
}

#endif // VIGIL_PLATFORM_LINUX_PROCFS_HPP
