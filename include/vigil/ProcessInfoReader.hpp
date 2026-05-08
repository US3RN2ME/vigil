
#ifndef VIGIL_PROCESSINFOREADER_HPP
#define VIGIL_PROCESSINFOREADER_HPP

#include <cstdint>
#include <optional>
#include <string>

#include <vigil/ProcessInfo.hpp>

namespace vigil {
   class ProcessInfoReader {
   public:
      virtual ~ProcessInfoReader() = default;

      [[nodiscard]] virtual std::optional<ProcessInfo> read(uint32_t pid) const = 0;
      [[nodiscard]] virtual std::optional<std::string> readName(uint32_t pid) const = 0;
   };
} // namespace vigil

#endif // VIGIL_PROCESSINFOREADER_HPP