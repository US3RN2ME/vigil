
#include "EventCollector.hpp"

#include <filesystem>
#include <format>
#include <fstream>

namespace vigil::common {
    [[nodiscard]] std::unique_ptr<IEventCollector> createEventCollector() {
        return std::make_unique<linux::EventCollector>();
    }
}

namespace vigil::linux {
    EventCollector::EventCollector() {
        try {
            bpfObj_.emplace("../bpf/execve.bpf.o");
            bpfObj_->load();
            bpfObj_->attach("onExecve");
            ringBuf_.emplace(bpfObj_->mapFd("rb"), onEvent, this);
            // return true;
        } catch (const std::runtime_error& e) {
            // log e.what()
            // return false;
        }
    }

    void EventCollector::start() {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            const auto name = entry.path().filename().string();
            const auto isDigit = [](const auto c) {
                return std::isdigit(c);
            };
            if (!std::all_of(name.begin(), name.end(), isDigit))
                continue;

            const auto pid = std::stoi(name);
            auto proc = readProcessInfo(pid);
            if (proc && callback_) {
                callback_(*proc);
            }
        }
    }

    void EventCollector::stop() {}

    int EventCollector::onEvent(void* ctx, void* data, size_t size) {}

    std::optional<common::ProcessInfo> EventCollector::readProcessInfo(int pid) {
        common::ProcessInfo p;
        p.pid = static_cast<uint32_t>(pid);

        const auto base = std::format("/proc/{}/", pid);

        try {
            p.exePath = std::filesystem::read_symlink(base + "exe").string();
            p.exeDeleted = p.exePath.ends_with("(deleted)");
            p.isMemfd = p.exePath.contains("/memfd:");
        } catch (...) {
            return std::nullopt;
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

        p.integrity = common::ProcessInfo::Integrity::Medium;

        return p;
    }
} // namespace vigil::linux