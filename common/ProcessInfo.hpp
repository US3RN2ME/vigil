
#ifndef VIGIL_COMMON_PROCESSINFO_HPP
#define VIGIL_COMMON_PROCESSINFO_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace vigil::common {
    /// Normalized cross-platform process snapshot.
    ///
    /// Populated by platform-specific collectors (LinuxCollector / WindowsCollector)
    /// and consumed by the platform-agnostic RuleEngine.
    ///
    /// Fields that have no equivalent on a given platform are set to their
    /// default values (false / 0 / Unknown) so that rules referencing them
    /// simply never fire — no #ifdef required in shared code.
    struct ProcessInfo {

        // ── Identity ─────────────────────────────────────────────────────────────

        /// Numeric process identifier.
        /// Linux : tgid (what userspace calls PID).
        /// Windows: UniqueProcessId from EPROCESS.
        uint32_t pid = 0;

        /// Numeric parent process identifier.
        /// Linux : task_struct.real_parent->tgid.
        /// Windows: InheritedFromUniqueProcessId from EPROCESS.
        ///          NOTE: can be spoofed via PROC_THREAD_ATTRIBUTE_PARENT_PROCESS.
        uint32_t ppid = 0;

        /// Short process name — limited by the kernel, not a reliable identifier.
        /// Linux : task_struct.comm, max 15 chars + NUL.
        /// Windows: EPROCESS.ImageFileName, max 15 chars + NUL.
        /// Use @ref exePath for a canonical identity check.
        std::string name;

        /// Absolute path to the on-disk binary, resolved through the kernel.
        /// Linux : readlink(/proc/<pid>/exe). Appends " (deleted)" if the file
        ///         has been unlinked from the filesystem after exec — see @ref exeDeleted.
        /// Windows: QueryFullProcessImageNameW with PROCESS_NAME_NATIVE flag.
        std::string exePath;

        /// Full command line including argv[0] and all arguments.
        /// Linux : /proc/<pid>/cmdline, NUL-separated, converted to space-separated.
        /// Windows: PEB.ProcessParameters.CommandLine (RTL_USER_PROCESS_PARAMETERS).
        std::string cmdline;

        // ── Ownership ────────────────────────────────────────────────────────────

        /// Real user identifier of the process owner.
        /// Linux : cred.uid.
        /// Windows: no direct equivalent — set to UINT32_MAX.
        uint32_t uid = UINT32_MAX;

        /// Effective user identifier used for permission checks.
        /// Linux : cred.euid. A transition uid=1000 → euid=0 indicates setuid exec
        ///         or a privilege escalation exploit.
        /// Windows: no direct equivalent — set to UINT32_MAX.
        uint32_t euid = UINT32_MAX;

        // ── Privileges ───────────────────────────────────────────────────────────

        /// Platform privilege bitmask.
        /// Linux : cred.cap_effective — each bit represents one POSIX capability
        ///         (CAP_SYS_ADMIN = bit 21, etc.). All bits set (0xFFFFFFFFFFFFFFFF)
        ///         means full root-equivalent access.
        /// Windows: bitmask derived from TOKEN_PRIVILEGES.Privileges[] — each bit
        ///          corresponds to one SE_PRIVILEGE (SE_DEBUG_PRIVILEGE, etc.).
        ///         Compare against a per-process baseline to detect escalation.
        uint64_t privilegeMask = 0;

        /// Windows-style mandatory integrity level.
        /// Linux : always set to Medium — the concept does not exist on Linux.
        ///         Rules that check for System-level integrity will never fire.
        /// Windows: derived from the process token's integrity SID
        ///          (Low / Medium / High / System).
        enum class Integrity : uint8_t {
            Low,
            Medium,
            High,
            System,
            Unknown ///< Collector could not read the token.
        };
        Integrity integrity = Integrity::Unknown;

        // ── Memory ───────────────────────────────────────────────────────────────

        /// Resident Set Size — physical RAM pages currently mapped.
        /// Linux : VmRSS from /proc/<pid>/status (converted to bytes).
        /// Windows: PROCESS_MEMORY_COUNTERS.WorkingSetSize.
        uint64_t rssBytes = 0;

        /// Virtual memory size — total address space reserved.
        /// Linux : VmSize from /proc/<pid>/status (converted to bytes).
        /// Windows: PROCESS_MEMORY_COUNTERS.PagefileUsage (committed virtual).
        uint64_t vszBytes = 0;

        /// Number of live threads inside the process.
        /// Linux : Threads from /proc/<pid>/status.
        /// Windows: NumberOfThreads from NtQueryInformationProcess.
        uint32_t threadCount = 0;

        // ── Anomaly flags (populated by Inspector) ───────────────────────────────

        /// The on-disk binary was unlinked after the process started.
        /// Classic fileless-malware indicator: payload executes then removes itself.
        /// Linux only — always false on Windows.
        bool exeDeleted = false;

        /// Process image was created via memfd_create() and never touched the disk.
        /// The exe path will contain "/memfd:" in this case.
        /// Linux only — always false on Windows.
        bool isMemfd = false;

        /// At least one anonymous (file-backed == false) memory region is mapped
        /// with both WRITE and EXECUTE permissions simultaneously.
        /// Strong indicator of shellcode injection or process hollowing.
        /// Linux : detected by parsing /proc/<pid>/maps for rwxp entries with no path.
        /// Windows: detected by VirtualQueryEx scanning for PAGE_EXECUTE_READWRITE.
        bool hasAnonRwx = false;

        /// LD_PRELOAD is set in the process environment.
        /// Indicates a shared-library hijack — a hook .so was preloaded into a
        /// legitimate process to intercept libc calls.
        /// Linux only — always false on Windows.
        bool hasLdPreload = false;

        /// The inode of the running binary does not match the inode of the file at
        /// exePath — the file was replaced on disk after the process started (TOCTOU).
        /// Linux only — always false on Windows.
        bool binaryReplaced = false;

        // ── Context ──────────────────────────────────────────────────────────────

        /// Container or isolation context the process belongs to.
        /// Linux : last entry of /proc/<pid>/cgroup (e.g. docker/<id>).
        /// Windows: name of the enclosing Job Object, if any.
        /// Empty string if the process runs on the bare host.
        std::string containerId;

        /// Process creation timestamp in nanoseconds since boot (monotonic clock).
        /// Used for event correlation and process-tree ordering.
        /// Linux : starttime field from /proc/<pid>/stat * (1e9 / CONFIG_HZ).
        /// Windows: EPROCESS.CreateTime converted to nanoseconds.
        uint64_t startTimeNs = 0;

        // ── Process tree (populated after full snapshot) ──────────────────────────

        /// Name of the parent process at snapshot time.
        /// Resolved from ppid after the full process list is collected.
        /// Empty if the parent exited before the snapshot was complete.
        std::string parentName;

        /// PIDs of direct child processes at snapshot time.
        /// Populated by the collector after the full process list is collected.
        std::vector<uint32_t> children;
    };
} // namespace vigil::common

#endif // VIGIL_COMMON_PROCESSINFO_HPP
