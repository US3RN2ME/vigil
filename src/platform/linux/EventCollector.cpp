
#include "EventCollector.hpp"

#include <optional>
#include <stdexcept>

#include "BpfEvents.hpp"
#include "ProcFs.hpp"

#include <vigil/Logger.hpp>

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::linux::EventCollector>();
   }
} // namespace vigil

namespace vigil::platform::linux {
   void EventCollector::start() {
      log::info("event collector starting");
      if (!init()) {
         log::error("event collector init failed, eBPF unavailable");
         return;
      }
      running_ = true;
      log::info("event collector running");
      while (running_) {
         const int result = ringBuf_->poll(100);
         if ((result < 0) && errno != EINTR) {
            log::error("ring buffer poll error: errno={}", errno);
            break;
         }
      }
   }

   void EventCollector::stop() {
      log::info("event collector stopping");
      running_ = false;
      ringBuf_.reset();
      bpf_.reset();
   }

   bool EventCollector::init() {
      try {
         bpf_.emplace(VIGIL_EVENTS_BPF_OBJECT);
         bpf_->load();
         bpf_->attach("onExecveEnter");
         bpf_->attach("onExecveExit");
         bpf_->attach("onMmapEnter");
         bpf_->attach("onMmapExit");
         bpf_->attach("onMprotectEnter");
         bpf_->attach("onMprotectExit");
         bpf_->attach("onConnect");
         bpf_->attach("onPtrace");
         bpf_->attach("onSetuid");
         bpf_->attach("onSetresuid");
         bpf_->attach("onInitModule");
         bpf_->attach("onFinitModule");
         ringBuf_.emplace(bpf_->mapFd("rb"), onEvent, this);
         return true;
      } catch (const std::runtime_error& e) {
         log::error("BPF init error: {}", e.what());
         return false;
      }
   }

   int EventCollector::onEvent(void* ctx, void* data, size_t /*size*/) {
      auto* self = static_cast<EventCollector*>(ctx);
      auto* event = static_cast<ExecveEvent*>(data);
      auto* hdr = static_cast<const EventHeader*>(data);

      switch (hdr->type) {
         case EventType::Execve:
            self->handleExecve(*static_cast<const ExecveEvent*>(data));
            break;
         case EventType::Mmap:
            self->handleMmap(*static_cast<const MmapEvent*>(data));
            break;
         case EventType::Connect:
            self->handleConnect(*static_cast<const ConnectEvent*>(data));
            break;
         case EventType::Ptrace:
            self->handlePtrace(*static_cast<const PtraceEvent*>(data));
            break;
         case EventType::Setuid:
            self->handleSetuid(*static_cast<const SetuidEvent*>(data));
            break;
         case EventType::Module:
            self->handleModule(*static_cast<const ModuleEvent*>(data));
            break;
      }
      return 0;
   }

   void EventCollector::handleExecve(const ExecveEvent& e) {
      auto proc = readProcessInfo(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      onProcess.emit(*proc);
   }

   void EventCollector::handleMmap(const MmapEvent& e) {
      auto proc = readProcessMaps(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      onProcess.emit(*proc);
   }

   void EventCollector::handleConnect(const ConnectEvent& e) {
      auto proc = readProcessInfo(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasConnect = true;
      proc->connectDport = e.dport;

      char buf[INET6_ADDRSTRLEN] = {};
      if (e.sa_family == AF_INET)
         ::inet_ntop(AF_INET, e.daddr, buf, INET_ADDRSTRLEN);
      else
         ::inet_ntop(AF_INET6, e.daddr, buf, INET6_ADDRSTRLEN);
      proc->connectDaddr = buf;

      onProcess.emit(*proc);
   }

   void EventCollector::handlePtrace(const PtraceEvent& e) {
      auto proc = readProcessInfo(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasPtraceAttach = true;
      proc->ptraceTargetPid = e.targetPid;
      onProcess.emit(*proc);
   }

   void EventCollector::handleSetuid(const SetuidEvent& e) {
      auto proc = readProcessInfo(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasSetuidToRoot = true;
      onProcess.emit(*proc);
   }

   void EventCollector::handleModule(const ModuleEvent& e) {
      auto proc = readProcessInfo(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasModuleLoad = true;
      onProcess.emit(*proc);
   }

} // namespace vigil::platform::linux
