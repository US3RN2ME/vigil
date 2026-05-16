#ifndef VIGIL_PLATFORM_WINDOWS_PROCESSPLATFORMINFO_HPP
#define VIGIL_PLATFORM_WINDOWS_PROCESSPLATFORMINFO_HPP

#include <cstdint>
#include <string>

namespace vigil::platform {
   /**
    * @brief Windows-specific process metadata.
    *
    * These fields are populated only by Windows collectors and are consumed by
    * Windows-specific rules or shared rules with Windows-specific branches.
    */
   struct ProcessPlatformInfo {
      /**
       * @brief Windows mandatory integrity level.
       */
      enum class Integrity : uint8_t {
         Low,
         Medium,
         High,
         System,
         /**
          * @brief Collector could not read the token.
          */
         Unknown
      };

      /**
       * @brief Integrity level observed for the process token.
       */
      Integrity integrity = Integrity::Unknown;

      /**
       * @brief Name of the enclosing Job Object, if any.
       */
      std::string jobObjectName;
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_WINDOWS_PROCESSPLATFORMINFO_HPP
