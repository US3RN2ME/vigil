
#ifndef VIGIL_PLATFORM_BPFEVENTS_HPP
#define VIGIL_PLATFORM_BPFEVENTS_HPP

#include <cstdint>

namespace vigil::platform {
enum class EventType : uint32_t {
  Execve = 1,
  Mmap = 2,
  Connect = 3,
  Ptrace = 4,
  Setuid = 5,
  Module = 6,
};

struct EventHeader {
  EventType type;
  uint32_t pid;
  uint32_t ppid;
  char comm[16];
};

struct ExecveEvent {
  EventHeader hdr;
  char filename[256];
};

struct MmapEvent {
  EventHeader hdr;
};

struct ConnectEvent {
  EventHeader hdr;
  uint16_t sa_family;
  uint16_t dport;
  uint8_t daddr[16];
};

struct PtraceEvent {
  EventHeader hdr;
  uint32_t targetPid;
  uint64_t request;
};

struct SetuidEvent {
  EventHeader hdr;
  uint32_t newUid;
};

struct ModuleEvent {
  EventHeader hdr;
};
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_BPFEVENTS_HPP
