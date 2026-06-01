#include "EventCollector.hpp"

#include "ProcessInfoReader.hpp"
#include "ProcessSnapshot.hpp"
#include "TcpTable.hpp"
#include "WinApi.hpp"

#include <vigil/Logger.hpp>
#include <vigil/SystemError.hpp>

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::EventCollector>(std::make_unique<platform::ProcessInfoReader>());
   }

} // namespace vigil

namespace vigil::platform {

   // Microsoft-Windows-Kernel-Process — available on Windows 8+.
   // Byte layout of GUID {0x22fb2cd6,0x0e7b,0x422b,{0xa0,0xc7,...}} in memory
   // (Data1/2/3 little-endian, Data4 verbatim).
   static constexpr std::array<uint8_t, 16> kKernelProcessProvider = {
       0xd6, 0x2c, 0xfb, 0x22, 0x7b, 0x0e, 0x2b, 0x42, 0xa0, 0xc7, 0x2f, 0xad, 0x1f, 0xd0, 0xe7, 0x16,
   };

   static constexpr USHORT kEventIdProcessStart = 1;
   static constexpr uint64_t kProcessKeyword = 0x10; // process-lifecycle only

   EventCollector::EventCollector(std::unique_ptr<vigil::ProcessInfoReader> reader)
       : vigil::EventCollector{std::move(reader)} {}

   void EventCollector::start() {
      log::info("event collector starting");
      if (!init()) {
         log::error("event collector init failed");
         workers_.stop();
         return;
      }

      if (stopRequested_) {
         stopEtw();
         workers_.stop();
         return;
      }

      running_ = true;
      log::info("event collector running");

      while (running_ && !stopRequested_) {
         if (stopEvent_.wait(100) == WaitResult::Signaled)
            break;

         const auto now = std::chrono::steady_clock::now();

         if (now >= nextScanTime_) {
            scanProcesses();
            nextScanTime_ = now + kScanInterval;
         }

         if (now >= nextNetScanTime_) {
            scanNetwork();
            nextNetScanTime_ = now + kNetScanInterval;
         }
      }

      running_ = false;
      stopEtw();
      workers_.stop();
   }

   void EventCollector::stop() {
      log::info("event collector stopping");
      stopRequested_ = true;
      running_ = false;

      {
         std::lock_guard lock(stopEventMutex_);
         stopEvent_.set();
      }
      stopEtw();
   }

   void EventCollector::stopEtw() {
      std::lock_guard lock(etwMutex_);
      if (etwSession_)
         etwSession_->stop();

      if (etwThread_.joinable())
         etwThread_.join();
   }

   void EventCollector::enqueueProcess(uint32_t pid) {
      workers_.submit(pid, [this, pid] {
         auto proc = processInfoReader_->read(pid);
         if (proc)
            onProcess.emit(*proc);
      });
   }

   bool EventCollector::init() {
      auto event = Event::create(true, false);
      if (!event) {
         log::error("CreateEvent failed: '{}'", error::lastMessage());
         return false;
      }

      {
         std::lock_guard lock(stopEventMutex_);
         stopEvent_ = std::move(*event);
      }

      const auto now = std::chrono::steady_clock::now();
      nextScanTime_ = now;
      nextNetScanTime_ = now;

      auto etwSession = EtwSession::start(L"vigil-kernel-process", kKernelProcessProvider, kProcessKeyword);
      if (!etwSession) {
         log::warn("ETW unavailable (requires elevation); running in scan-only mode");
      } else {
         std::lock_guard lock(etwMutex_);
         etwSession_ = std::move(etwSession);
         etwThread_ = std::thread([this] {
            etwSession_->consume([this](const _EVENT_RECORD& r) {
               if (r.EventHeader.EventDescriptor.Id != kEventIdProcessStart)
                  return;
               const auto pid = static_cast<uint32_t>(r.EventHeader.ProcessId);
               enqueueProcess(pid);
            });
         });
      }

      return true;
   }

   void EventCollector::scanProcesses() {
      log::debug("process scan starting");
      int count = 0;

      const auto snapshot = ProcessSnapshot::create();
      if (!snapshot) {
         log::error("CreateToolhelp32Snapshot failed");
         return;
      }

      snapshot->forEach([&](uint32_t pid) {
         enqueueProcess(pid);
         ++count;
      });

      log::debug("process scan complete: {} processes", count);
   }

   void EventCollector::scanNetwork() {
      TcpTable::forEach([&](const TcpConnection& conn) {
         const auto key = std::to_string(conn.pid) + ':' + conn.remoteAddr + ':' + std::to_string(conn.remotePort);

         if (!seenConnections_.insert(key).second)
            return;

         workers_.submit(conn.pid, [this, conn] {
            auto proc = processInfoReader_->read(conn.pid);
            if (!proc)
               return;

            proc->hasConnect = true;
            proc->connectDport = conn.remotePort;
            proc->connectDaddr = conn.remoteAddr;
            onProcess.emit(*proc);
         });
      });
   }

} // namespace vigil::platform
