
#include "EventCollector.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <filesystem>
#include <optional>
#include <stdexcept>

#include "BpfEvents.hpp"
#include "ProcessInfoReader.hpp"

#include <vigil/Logger.hpp>
#include <vigil/SystemError.hpp>

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::EventCollector>(std::make_unique<platform::ProcessInfoReader>());
   }
} // namespace vigil

namespace vigil::platform {
   EventCollector::EventCollector(std::unique_ptr<vigil::ProcessInfoReader> reader)
       : vigil::EventCollector{std::move(reader)} {}

   void EventCollector::start() {
      log::info("event collector starting");
      if (!init()) {
         log::error("event collector init failed, eBPF unavailable");
         workers_.stop();
         return;
      }

      if (stopRequested_) {
         workers_.stop();
         ringBuf_.reset();
         bpf_.reset();
         return;
      }

      running_ = true;
      log::info("event collector running");

      while (running_ && !stopRequested_) {
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

      running_ = false;
      workers_.stop();
      ringBuf_.reset();
      bpf_.reset();
   }

   void EventCollector::stop() {
      log::info("event collector stopping");
      stopRequested_ = true;
      running_ = false;
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
               if (fname.empty() || !std::ranges::all_of(fname, [](unsigned char ch) {
                      return std::isdigit(ch);
                   }))
                  continue;
               const int pid = std::stoi(fname);
               workers_.submit(pid, [this, pid] {
                  auto proc = processInfoReader_->read(pid);
                  if (proc)
                     onProcess.emit(*proc);
               });
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
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const ExecveEvent*>(data)] {
               self->handleExecve(event);
            });
            break;
         case EventType::Mmap:
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const MmapEvent*>(data)] {
               self->handleMmap(event);
            });
            break;
         case EventType::Connect:
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const ConnectEvent*>(data)] {
               self->handleConnect(event);
            });
            break;
         case EventType::Ptrace:
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const PtraceEvent*>(data)] {
               self->handlePtrace(event);
            });
            break;
         case EventType::Setuid:
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const SetuidEvent*>(data)] {
               self->handleSetuid(event);
            });
            break;
         case EventType::Module:
            self->workers_.submit(hdr->pid, [self, event = *static_cast<const ModuleEvent*>(data)] {
               self->handleModule(event);
            });
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
      proc->platform.hasPtraceAttach = true;
      proc->platform.ptraceTargetPid = e.targetPid;
      onProcess.emit(*proc);
   }

   void EventCollector::handleSetuid(const SetuidEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->platform.hasSetuidToRoot = true;
      onProcess.emit(*proc);
   }

   void EventCollector::handleModule(const ModuleEvent& e) {
      auto proc = processInfoReader_->read(static_cast<int>(e.hdr.pid));
      if (!proc)
         return;
      proc->ppid = e.hdr.ppid;
      proc->name = e.hdr.comm;
      proc->platform.hasModuleLoad = true;
      onProcess.emit(*proc);
   }

} // namespace vigil::platform
