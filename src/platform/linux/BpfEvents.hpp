
#ifndef VIGIL_PLATFORM_LINUX_BPFEVENTS_HPP
#define VIGIL_PLATFORM_LINUX_BPFEVENTS_HPP

#include <cstdint>

namespace vigil::platform::linux {
   struct ExecveEvent {
      uint32_t pid;
      uint32_t ppid;
      char comm[16];
      char filename[256];
   };

   struct MmapEvent {
      uint32_t pid;
      uint32_t ppid;
      char comm[16];
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_BPFEVENTS_HPP
