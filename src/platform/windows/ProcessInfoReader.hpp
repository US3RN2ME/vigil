
#ifndef VIGIL_PLATFORM_PROCESSINFOREADER_HPP
#define VIGIL_PLATFORM_PROCESSINFOREADER_HPP

#include <cstdint>
#include <optional>
#include <string>

#include "vigil/ProcessInfoReader.hpp"

namespace vigil::platform {
   class ProcessInfoReader : public vigil::ProcessInfoReader {
   public:
      [[nodiscard]] std::optional<ProcessInfo> read(uint32_t pid) const override;
      [[nodiscard]] std::optional<std::string> readName(uint32_t pid) const override;
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_PROCESSINFOREADER_HPP
