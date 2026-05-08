
#ifndef VIGIL_PLATFORM_LINUX_PROCESSINFOREADER_HPP
#define VIGIL_PLATFORM_LINUX_PROCESSINFOREADER_HPP

#include <vigil/ProcessInfoReader.hpp>

namespace vigil::platform::linux {
   class ProcessInfoReader : public vigil::ProcessInfoReader {
   public:
      [[nodiscard]] std::optional<ProcessInfo> read(uint32_t pid) const override;
      [[nodiscard]] std::optional<std::string> readName(uint32_t pid) const override;
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_PROCESSINFOREADER_HPP