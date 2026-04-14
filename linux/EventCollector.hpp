
#ifndef VIGIL_LINUX_EVENTCOLLECTOR_HPP
#define VIGIL_LINUX_EVENTCOLLECTOR_HPP

#include <atomic>
#include <optional>

#include "BpfObject.hpp"
#include "RingBuffer.hpp"
#include "common/IEventCollector.hpp"
#include "common/ProcessInfo.hpp"

namespace vigil::linux {
    struct ExecveEvent {
        uint32_t pid;
        uint32_t ppid;
        char comm[16];
        char filename[256];
    };

    class EventCollector : public common::IEventCollector {
    public:
        EventCollector();
        void start() override;
        void stop() override;

    private:
        static int onEvent(void* ctx, void* data, size_t size);
        static std::optional<common::ProcessInfo> readProcessInfo(int pid);

        std::optional<BpfObject> bpfObj_;
        std::optional<RingBuffer> ringBuf_;
        std::atomic<bool> running_{false};
    };
} // namespace vigil::linux

#endif // VIGIL_LINUX_EVENTCOLLECTOR_HPP
