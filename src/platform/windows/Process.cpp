#include "Process.hpp"

#include "WinApi.hpp"

namespace vigil::platform::windows {
   Handle Process::open(uint32_t pid) {
      Handle handle{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid)};

      if (!handle) {
         handle.reset(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
      }

      return handle;
   }
} // namespace vigil::platform::windows
