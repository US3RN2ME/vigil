#ifndef VIGIL_PLATFORM_PROCESSPLATFORMINFO_INTERNAL_HPP
#define VIGIL_PLATFORM_PROCESSPLATFORMINFO_INTERNAL_HPP

#include <cstdint>
#include <string>

namespace vigil::platform {
   /**
    * @brief Linux-specific process metadata.
    *
    * These fields are populated only by Linux collectors and are consumed by
    * Linux-specific rules or shared rules with Linux-specific branches.
    */
   struct ProcessPlatformInfo {
      /**
       * @brief Real user identifier of the process owner.
       */
      uint32_t uid = UINT32_MAX;

      /**
       * @brief Effective user identifier used for permission checks.
       */
      uint32_t euid = UINT32_MAX;

      /**
       * @brief The on-disk binary was unlinked after the process started.
       */
      bool exeDeleted = false;

      /**
       * @brief Process image was created via memfd_create() and never touched the disk.
       */
      bool isMemfd = false;

      /**
       * @brief LD_PRELOAD is set in the process environment.
       */
      bool hasLdPreload = false;

      /**
       * @brief Running binary inode differs from the inode at exePath.
       */
      bool binaryReplaced = false;

      /**
       * @brief Container or cgroup context the process belongs to.
       */
      std::string containerId;

      /**
       * @brief Process called ptrace(PTRACE_ATTACH) or ptrace(PTRACE_SEIZE).
       */
      bool hasPtraceAttach = false;

      /**
       * @brief PID of the process targeted by the ptrace call.
       */
      uint32_t ptraceTargetPid = 0;

      /**
       * @brief A non-root process called setuid(0) or equivalent.
       */
      bool hasSetuidToRoot = false;

      /**
       * @brief Process invoked init_module(2) or finit_module(2).
       */
      bool hasModuleLoad = false;
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_PROCESSPLATFORMINFO_INTERNAL_HPP
