#include "ProcessInfoReader.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

#include <vigil/Logger.hpp>

namespace vigil::platform {
namespace {

bool readExe(ProcessInfo &p, const std::string &base) {
  try {
    p.exePath = std::filesystem::read_symlink(base + "exe").string();
    p.platform.exeDeleted = p.exePath.ends_with("(deleted)");
    p.imageMissingFromDisk = p.platform.exeDeleted;
    p.platform.isMemfd = p.exePath.find("/memfd:") != std::string::npos;
  } catch (...) {
    return false;
  }

  if (!p.platform.exeDeleted && !p.platform.isMemfd) {
    struct ::stat linkStat{};
    struct ::stat pathStat{};
    if (::stat((base + "exe").c_str(), &linkStat) == 0 &&
        ::stat(p.exePath.c_str(), &pathStat) == 0)
      p.platform.binaryReplaced = linkStat.st_ino != pathStat.st_ino;
  }
  return true;
}

std::optional<std::string> readProcessName(uint32_t pid) {
  std::ifstream f{"/proc/" + std::to_string(pid) + "/status"};
  if (!f)
    return {};

  for (std::string line; std::getline(f, line);) {
    if (!line.starts_with("Name:"))
      continue;
    const auto start = line.find_first_not_of(" \t", 5);
    return start != std::string::npos ? line.substr(start) : std::string{};
  }
  return {};
}

bool readStatus(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "status"};
  if (!f)
    return false;

  const auto parseValue = [](const std::string &line) -> std::string {
    const auto colon = line.find(':');
    if (colon == std::string::npos)
      return {};
    const auto start = line.find_first_not_of(" \t", colon + 1);
    return start != std::string::npos ? line.substr(start) : std::string{};
  };

  for (std::string line; std::getline(f, line);) {
    const auto val = parseValue(line);
    try {
      if (line.starts_with("Name:"))
        p.name = val;
      else if (line.starts_with("PPid:")) {
        p.ppid = static_cast<uint32_t>(std::stoul(val));
        if (auto name = readProcessName(p.ppid))
          p.parentName = std::move(*name);
      } else if (line.starts_with("Threads:"))
        p.threadCount = static_cast<uint32_t>(std::stoul(val));
      else if (line.starts_with("VmRSS:"))
        p.rssBytes = std::stoull(val) * 1024ULL;
      else if (line.starts_with("VmSize:"))
        p.vszBytes = std::stoull(val) * 1024ULL;
      else if (line.starts_with("CapEff:"))
        p.privilegeMask = std::stoull(val, nullptr, 16);
      else if (line.starts_with("Uid:")) {
        std::istringstream ss{val};
        ss >> p.platform.uid >> p.platform.euid;
      }
    } catch (const std::exception &) {
    }
  }
  return true;
}

void readCmdline(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "cmdline"};
  if (!f)
    return;
  std::string raw{std::istreambuf_iterator{f}, {}};
  std::ranges::replace(raw, '\0', ' ');
  p.cmdline = std::move(raw);
}

void readMapsFile(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "maps"};
  if (!f)
    return;
  for (std::string line; std::getline(f, line);) {
    std::istringstream ss{line};
    std::string addr, perms, offset, dev, inode, path;
    ss >> addr >> perms >> offset >> dev >> inode >> path;
    if (perms.find('w') != std::string::npos &&
        perms.find('x') != std::string::npos && path.empty()) {
      p.hasAnonRwx = true;
      break;
    }
  }
}

void readEnviron(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "environ"};
  if (!f)
    return;
  const std::string raw{std::istreambuf_iterator{f}, {}};
  p.platform.hasLdPreload = raw.find("LD_PRELOAD=") != std::string::npos;
}

void readCgroup(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "cgroup"};
  if (!f)
    return;

  for (std::string line; std::getline(f, line);) {
    const auto pathStart = line.find_last_of(':');
    if (pathStart == std::string::npos)
      continue;

    const auto path = line.substr(pathStart + 1);
    if (path.find("docker") != std::string::npos ||
        path.find("kubepods") != std::string::npos ||
        path.find("containerd") != std::string::npos ||
        path.find("libpod") != std::string::npos ||
        path.find("lxc") != std::string::npos) {
      p.platform.containerId = path;
      return;
    }
  }
}

void readStartTime(ProcessInfo &p, const std::string &base) {
  std::ifstream f{base + "stat"};
  std::string line;
  if (!f || !std::getline(f, line))
    return;

  const auto commEnd = line.rfind(')');
  if (commEnd == std::string::npos || commEnd + 2 >= line.size())
    return;

  std::istringstream fields{line.substr(commEnd + 2)};
  std::string ignored;
  for (int field = 3; field < 22; ++field) {
    if (!(fields >> ignored))
      return;
  }

  uint64_t startTicks = 0;
  if (!(fields >> startTicks))
    return;

  const auto ticksPerSecond = ::sysconf(_SC_CLK_TCK);
  if (ticksPerSecond > 0)
    p.startTimeNs =
        startTicks * 1'000'000'000ULL / static_cast<uint64_t>(ticksPerSecond);
}

} // namespace

std::optional<ProcessInfo> ProcessInfoReader::read(uint32_t pid) const {
  ProcessInfo p;
  p.pid = pid;
  const auto base = "/proc/" + std::to_string(pid) + "/";

  if (!readExe(p, base)) {
    log::debug("process {} vanished before snapshot", pid);
    return {};
  }

  if (!readStatus(p, base)) {
    log::debug("process {} inaccessible or vanished", pid);
    return {};
  }

  readCmdline(p, base);
  readMapsFile(p, base);
  readEnviron(p, base);
  readCgroup(p, base);
  readStartTime(p, base);

  return p;
}

std::optional<std::string> ProcessInfoReader::readName(uint32_t pid) const {
  return readProcessName(pid);
}

} // namespace vigil::platform
