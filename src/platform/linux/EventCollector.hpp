
#ifndef VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
#define VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP

#include <atomic>
#include <optional>

#include "BpfObject.hpp"
#include "RingBuffer.hpp"

#include <vigil/EventCollector.hpp>

namespace vigil::platform::linux {
   class EventCollector : public vigil::EventCollector {
   public:
      void start() override;
      void stop() override;

   private:
      bool init();
      static int onEvent(void* ctx, void* data, size_t size);
      static int onMmapEvent(void* ctx, void* data, size_t size);

      std::optional<BpfObject> bpfMmap_;
      std::optional<RingBuffer> mmapRingBuf_;
      std::optional<BpfObject> bpfExecve_;
      std::optional<RingBuffer> execveRingBuf_;
      std::atomic<bool> running_{false};
   };
} // namespace vigil::platform::linux

#endif // VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
