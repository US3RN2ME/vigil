
#ifndef VIGIL_PLATFORM_WINDOWS_PROCESS_HPP
#define VIGIL_PLATFORM_WINDOWS_PROCESS_HPP

#include <cstdint>

#include "Handle.hpp"

namespace vigil::platform::windows {
   class Process {
   public:
      [[nodiscard]] static Handle open(uint32_t pid);
   };
} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_WINDOWS_PROCESS_HPP
