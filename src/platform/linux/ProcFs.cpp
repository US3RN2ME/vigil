
#include "ProcFs.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include <vigil/Logger.hpp>

namespace vigil::platform::linux {
   std::optional<ProcessInfo> readProcessInfo(int pid) {
      ProcessInfo p;
      p.pid = static_cast<uint32_t>(pid);
      const auto base = std::string("/proc/") + std::to_string(pid) + "/";

      try {
         p.exePath = std::filesystem::read_symlink(base + "exe").string();
         p.exeDeleted = p.exePath.ends_with("(deleted)");
         p.isMemfd = p.exePath.contains("/memfd:");
      } catch (...) {
         log::debug("process {} vanished before snapshot", pid);
         return {};
      }

      if (!p.exeDeleted && !p.isMemfd) {
         struct stat exeLinkStat{};
         struct stat exePathStat{};

         const auto exeLink = base + "exe";
         if (stat(exeLink.c_str(), &exeLinkStat) == 0 && stat(p.exePath.c_str(), &exePathStat) == 0) {
            p.binaryReplaced = exeLinkStat.st_ino != exePathStat.st_ino;
         }
      }

      if (std::ifstream f{base + "cmdline"}; f) {
         std::string raw{std::istreambuf_iterator{f}, {}};
         std::replace(raw.begin(), raw.end(), '\0', ' ');
         p.cmdline = std::move(raw);
      }

      if (std::ifstream f{base + "status"}; f) {
         const auto parseValue = [](const std::string& line) -> std::string {
            const auto colon = line.find(':');
            if (colon == std::string::npos)
               return {};
            const auto start = line.find_first_not_of(" \t", colon + 1);
            return start != std::string::npos ? line.substr(start) : std::string{};
         };

         for (std::string line; std::getline(f, line);) {
            const auto val = parseValue(line);

            if (line.starts_with("Name:"))
               p.name = val;
            else if (line.starts_with("PPid:"))
               p.ppid = static_cast<uint32_t>(std::stoul(val));
            else if (line.starts_with("Threads:"))
               p.threadCount = static_cast<uint32_t>(std::stoul(val));
            else if (line.starts_with("VmRSS:"))
               p.rssBytes = std::stoull(val) * 1024ULL;
            else if (line.starts_with("VmSize:"))
               p.vszBytes = std::stoull(val) * 1024ULL;
            else if (line.starts_with("CapEff:"))
               p.privilegeMask = std::stoull(val, nullptr, 16);
            else if (line.starts_with("Uid:")) {
               std::istringstream ss{val};
               ss >> p.uid >> p.euid;
            }
         }
      }

      if (std::ifstream f{base + "maps"}; f) {
         for (std::string line; std::getline(f, line);) {
            std::istringstream ss{line};
            std::string addr, perms, offset, dev, inode, path;
            ss >> addr >> perms >> offset >> dev >> inode >> path;

            const bool isExecutable = perms.contains('x');
            const bool isWritable = perms.contains('w');
            const bool isAnonymous = path.empty();

            if (isExecutable && isWritable && isAnonymous) {
               p.hasAnonRwx = true;
               break;
            }
         }
      }

      if (std::ifstream f{base + "environ"}; f) {
         const std::string raw{std::istreambuf_iterator{f}, {}};
         p.hasLdPreload = raw.contains("LD_PRELOAD=");
      }

      if (std::ifstream f{base + "cgroup"}; f)
         std::getline(f, p.containerId);

      p.integrity = ProcessInfo::Integrity::Medium;

      return p;
   }
} // namespace vigil::platform::linux
