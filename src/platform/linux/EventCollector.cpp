
#include "EventCollector.hpp"

#include <arpa/inet.h>
#include <filesystem>
#include <optional>
#include <stdexcept>

#include "BpfEvents.hpp"
#include "ProcFs.hpp"
#include "ProcessInfoReader.hpp"
#include "vigil/SystemError.hpp"

#include <vigil/Logger.hpp>

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::linux::EventCollector>(std::make_unique<platform::linux::ProcessInfoReader>());
   }
} // namespace vigil

namespace vigil::platform::linux {
   EventCollector::EventCollector(std::unique_ptr<vigil::ProcessInfoReader> reader) {}
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
         if ((result < 0) && error::lastCode() != EINTR) {
            log::error("ring buffer poll error: error:'{}'", error::lastMessage());
            break;
         }
         const auto now = std::chrono::steady_clock::now();
         if (now >= nextScanTime_) {
            scanProc();
            nextScanTime_ = now + kScanInterval;
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
         bpf_.emplace();
         bpf_->load();
         ringBuf_.emplace(bpf_->ringBufFd(), onEvent, this);
         return true;
      } catch (const std::runtime_error& e) {
         log::error("BPF init error: {}", e.what());
         return false;
      }
   }

   void EventCollector::scanProc() {
      log::debug("proc scan starting");
      int count = 0;
      try {
         for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            try {
               if (!entry.is_directory())
                  continue;
               const auto fname = entry.path().filename().string();
               if (fname.empty() || !std::all_of(fname.begin(), fname.end(), ::isdigit))
                  continue;
               const int pid = std::stoi(fname);
               auto proc = processInfoReader_->read(pid);
               if (!proc)
                  continue;
               onProcess.emit(*proc);
               ++count;
            } catch (const std::exception& ex) {
               log::debug("proc scan skipped entry: {}", ex.what());
            }
         }
      } catch (const std::exception& ex) {
         log::error("proc scan failed: {}", ex.what());
      }
      log::debug("proc scan complete: {} processes", count);
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
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      onProcess.emit(*proc);
   }

   void EventCollector::handleMmap(const MmapEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      onProcess.emit(*proc);
   }

   void EventCollector::handleConnect(const ConnectEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
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
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasPtraceAttach = true;
      proc->ptraceTargetPid = e.targetPid;
      onProcess.emit(*proc);
   }

   void EventCollector::handleSetuid(const SetuidEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasSetuidToRoot = true;
      onProcess.emit(*proc);
   }

   void EventCollector::handleModule(const ModuleEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->hasModuleLoad = true;
      onProcess.emit(*proc);
   }

} // namespace vigil::platform::linux
