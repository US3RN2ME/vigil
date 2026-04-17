
#include "EventCollector.hpp"

#include <optional>
#include <stdexcept>

#include "BpfEvents.hpp"
#include "ProcFs.hpp"

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::linux::EventCollector>();
   }
} // namespace vigil

namespace vigil::platform::linux {
   void EventCollector::start() {
      if (!init())
         return;
      running_ = true;
      while (running_) {
         const int result = ringBuf_->poll(100);
         if (result < 0 && errno != EINTR)
            break;
      }
   }

   void EventCollector::stop() {
      running_ = false;
      ringBuf_.reset();
      bpfObj_.reset();
   }

   bool EventCollector::init() {
      try {
         bpfObj_.emplace(VIGIL_EXECVE_BPF_OBJECT);
         bpfObj_->load();
         bpfObj_->attach("onExecve");
         ringBuf_.emplace(bpfObj_->mapFd("rb"), onEvent, this);
         return true;
      } catch (const std::runtime_error&) {
         return false;
      }
   }

   int EventCollector::onEvent(void* ctx, void* data, size_t /*size*/) {
      auto* self = static_cast<EventCollector*>(ctx);
      auto* event = static_cast<ExecveEvent*>(data);

      auto proc = readProcessInfo(static_cast<int>(event->pid));
      if (!proc)
         return 0;

      proc->ppid = event->ppid;
      proc->name = event->comm;

      self->onProcess.emit(*proc);
      return 0;
   }
} // namespace vigil::platform::linux
