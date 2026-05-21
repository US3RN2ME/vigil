
#ifndef VIGIL_PROCESSINFO_HPP
#define VIGIL_PROCESSINFO_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <vigil/platform/ProcessPlatformInfo.hpp>

namespace vigil {
   /**
    * @brief Normalized cross-platform process snapshot.
    *
    * Populated by platform-specific collectors (LinuxCollector / WindowsCollector)
    * and consumed by the platform-agnostic RuleEngine.
    *
    * Common fields live directly on ProcessInfo. Platform-only metadata lives
    * in @ref platform and is selected by the
    * target platform at compile time.
    */
   struct ProcessInfo {
      // ── Identity ─────────────────────────────────────────────────────────────

      /**
       * @brief Numeric process identifier.
       * Linux : tgid (what userspace calls PID).
       * Windows: UniqueProcessId from EPROCESS.
       */
      uint32_t pid = 0;

      /**
       * @brief Numeric parent process identifier.
       * Linux : task_struct.real_parent->tgid.
       * Windows: InheritedFromUniqueProcessId from EPROCESS.
       *          NOTE: can be spoofed via PROC_THREAD_ATTRIBUTE_PARENT_PROCESS.
       */
      uint32_t ppid = 0;

      /**
       * @brief Short process name — limited by the kernel, not a reliable identifier.
       * Linux : task_struct.comm, max 15 chars + NUL.
       * Windows: EPROCESS.ImageFileName, max 15 chars + NUL.
       * Use @ref exePath for a canonical identity check.
       */
      std::string name;

      /**
       * @brief Absolute path to the on-disk binary, resolved through the kernel.
       * Linux :
       * readlink(/proc/&lt;pid&gt;/exe). Appends " (deleted)" if the file
       *         has been unlinked from the
       * filesystem after exec.
       * Windows: QueryFullProcessImageNameW with PROCESS_NAME_NATIVE flag.
       */
      std::string exePath;

      /**
       * @brief Full command line including argv[0] and all arguments.
       * Linux : /proc/&lt;pid&gt;/cmdline, NUL-separated, converted to space-separated.
       * Windows: PEB.ProcessParameters.CommandLine (RTL_USER_PROCESS_PARAMETERS).
       */
      std::string cmdline;

      // ── Privileges ───────────────────────────────────────────────────────────

      /**
       * @brief Platform privilege bitmask.
       * Linux : cred.cap_effective — each bit represents one POSIX capability
       *         (CAP_SYS_ADMIN = bit 21, etc.). All bits set (0xFFFFFFFFFFFFFFFF)
       *         means full root-equivalent access.
       * Windows: bitmask derived from TOKEN_PRIVILEGES.Privileges[] — each bit
       *          corresponds to one SE_PRIVILEGE (SE_DEBUG_PRIVILEGE, etc.).
       *         Compare against a per-process baseline to detect escalation.
       */
      uint64_t privilegeMask = 0;

      // ── Memory ───────────────────────────────────────────────────────────────

      /**
       * @brief Resident Set Size — physical RAM pages currently mapped.
       * Linux : VmRSS from /proc/&lt;pid&gt;/status (converted to bytes).
       * Windows: PROCESS_MEMORY_COUNTERS.WorkingSetSize.
       */
      uint64_t rssBytes = 0;

      /**
       * @brief Virtual memory size — total address space reserved.
       * Linux : VmSize from /proc/&lt;pid&gt;/status (converted to bytes).
       * Windows: PROCESS_MEMORY_COUNTERS.PagefileUsage (committed virtual).
       */
      uint64_t vszBytes = 0;

      /**
       * @brief Number of live threads inside the process.
       * Linux : Threads from /proc/&lt;pid&gt;/status.
       * Windows: NumberOfThreads from NtQueryInformationProcess.
       */
      uint32_t threadCount = 0;

      // ── Anomaly flags (populated by Inspector) ───────────────────────────────

      /**
       * @brief The executable path resolved for the process no longer exists on disk.
       * Linux: true for
       * deleted executables. Windows: true when the image path
       * cannot be found during the snapshot. This is a
       * high-signal image
       * backing anomaly, but not proof of maliciousness by itself.
       */
      bool imageMissingFromDisk = false;

      /**
       * @brief At least one anonymous (file-backed == false) memory region is mapped
       * with both WRITE and
       * EXECUTE permissions simultaneously. Strong indicator of shellcode injection or process hollowing.
       * Linux :
       * detected by parsing /proc/&lt;pid&gt;/maps for rwxp entries with no path.
       * Windows: detected by VirtualQueryEx
       * scanning for PAGE_EXECUTE_READWRITE.
       */
      bool hasAnonRwx = false;

      /**
       * @brief Process creation timestamp in nanoseconds since boot (monotonic clock).
       * Used for event
       * correlation and process-tree ordering. Linux : starttime field from /proc/&lt;pid&gt;/stat * (1e9 / CONFIG_HZ).
       * Windows: EPROCESS.CreateTime converted to nanoseconds.
       */
      uint64_t startTimeNs = 0;

      // ── Process tree (populated after full snapshot) ──────────────────────────

      /**
       * @brief Name of the parent process at snapshot time.
       * Resolved from ppid after the full process list is collected.
       * Empty if the parent exited before the snapshot was complete.
       */
      std::string parentName;

      /**
       * @brief PIDs of direct child processes at snapshot time.
       * Populated by the collector after the full process list is collected.
       */
      std::vector<uint32_t> children;

      // ── Network ──────────────────────────────────────────────────────────────

      /**
       * @brief Process called connect(2) to a non-loopback AF_INET/AF_INET6 address.
       */
      bool hasConnect = false;

      /**
       * @brief Destination port of the connect(2) call, in host byte order.
       */
      uint16_t connectDport = 0;

      /**
       * @brief Destination address, formatted as a dotted-decimal (IPv4) or
       * colon-hex (IPv6) string.
       */
      std::string connectDaddr;

      /**
       * @brief Concrete platform metadata type selected for the current target OS.
       */
      using PlatformInfo = platform::ProcessPlatformInfo;

      /**
       * @brief Platform-specific process metadata for the current target OS.
       */
      PlatformInfo platform;
   };
} // namespace vigil

#endif // VIGIL_PROCESSINFO_HPP
