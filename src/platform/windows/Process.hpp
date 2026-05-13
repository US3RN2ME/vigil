
#ifndef VIGIL_PLATFORM_PROCESS_HPP
#define VIGIL_PLATFORM_PROCESS_HPP

#include <cstdint>

#include "Handle.hpp"

namespace vigil::platform {
   class Process {
   public:
      [[nodiscard]] static Handle open(uint32_t pid);
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_PROCESS_HPP
