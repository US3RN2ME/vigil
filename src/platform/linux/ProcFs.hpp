
#ifndef VIGIL_PLATFORM_LINUX_PROCFS_HPP
#define VIGIL_PLATFORM_LINUX_PROCFS_HPP

#include <optional>

#include <vigil/ProcessInfo.hpp>

namespace vigil::platform::linux {
   bool readExe(ProcessInfo& p, const std::string& base);
   bool readStatus(ProcessInfo& p, const std::string& base);
   void readCmdline(ProcessInfo& p, const std::string& base);
   void readMaps(ProcessInfo& p, const std::string& base);
   void readEnviron(ProcessInfo& p, const std::string& base);
   void readCgroup(ProcessInfo& p, const std::string& base);

   std::optional<ProcessInfo> readProcessInfo(int pid);
   std::optional<ProcessInfo> readProcessMaps(int pid);
   std::optional<std::string> readProcessName(int pid);
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_PROCFS_HPP
