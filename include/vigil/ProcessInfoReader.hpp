
#ifndef VIGIL_PROCESSINFOREADER_HPP
#define VIGIL_PROCESSINFOREADER_HPP

#include <cstdint>
#include <optional>
#include <string>

#include <vigil/ProcessInfo.hpp>

namespace vigil {
   /**
    * @brief Reads point-in-time process metadata from the operating system.
    */
   class ProcessInfoReader {
   public:
      virtual ~ProcessInfoReader() = default;

      /**
       * @brief Read a full normalized process snapshot.
       *
       * @param pid Process identifier to read.
    *

       * * @return std::nullopt when the process no longer exists or cannot be read.
       */
      [[nodiscard]] virtual std::optional<ProcessInfo> read(uint32_t pid) const = 0;

      /**
       * @brief Read only the short process name for a PID.
       *
       * @param pid Process identifier to read.

       * *
       * @return std::nullopt when the process no longer exists or cannot be read.
       */
      [[nodiscard]] virtual std::optional<std::string> readName(uint32_t pid) const = 0;
   };
} // namespace vigil

#endif // VIGIL_PROCESSINFOREADER_HPP
