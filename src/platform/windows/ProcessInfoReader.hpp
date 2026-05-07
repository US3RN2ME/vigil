
#ifndef VIGIL_PROCESSINFOREADER_HPP
#define VIGIL_PROCESSINFOREADER_HPP

#include <cstdint>
#include <optional>
#include <string>

#include "ProcessSnapshot.hpp"

namespace vigil::platform::windows {
   class ProcessInfoReader {
   public:
      [[nodiscard]] std::optional<ProcessInfo> read(uint32_t pid) const;
      [[nodiscard]] std::optional<std::string> readName(uint32_t pid) const;

   private:
   };
} // namespace vigil::platform::windows

#endif // VIGIL_PROCESSINFOREADER_HPP
