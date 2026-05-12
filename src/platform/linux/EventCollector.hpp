
#ifndef VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
#define VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP

#include <atomic>
#include <chrono>
#include <optional>

#include "BpfEvents.hpp"
#include "BpfObject.hpp"
#include "RingBuffer.hpp"

#include <vigil/EventCollector.hpp>
#include <vigil/ProcessInfoReader.hpp>

namespace vigil::platform::linux {
   class EventCollector : public vigil::EventCollector {
      static constexpr std::chrono::seconds kScanInterval{30};

   public:
      explicit EventCollector(std::unique_ptr<vigil::ProcessInfoReader> reader);

      void start() override;
      void stop() override;

   private:
      bool init();
      static int onEvent(void* ctx, void* data, size_t size);

      void handleExecve(const ExecveEvent& e);
      void handleMmap(const MmapEvent& e);
      void handleConnect(const ConnectEvent& e);
      void handlePtrace(const PtraceEvent& e);
      void handleSetuid(const SetuidEvent& e);
      void handleModule(const ModuleEvent& e);
      void scanProc();

      std::optional<BpfObject> bpf_;
      std::optional<RingBuffer> ringBuf_;
      std::atomic<bool> running_{false};
      std::chrono::steady_clock::time_point nextScanTime_{std::chrono::steady_clock::now()};
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
