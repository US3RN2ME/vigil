
#ifndef VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP
#define VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP

#include <cstdint>
#include <functional>
#include <optional>

#include "Handle.hpp"

namespace vigil::platform::windows {

   class ProcessSnapshot {
   public:
      explicit ProcessSnapshot(Handle handle) noexcept;

      static std::optional<ProcessSnapshot> create();

      void forEach(const std::function<void(uint32_t pid)>& callback) const;

   private:
      Handle handle_;
   };

} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_WINDOWS_PROCESSSNAPSHOT_HPP
