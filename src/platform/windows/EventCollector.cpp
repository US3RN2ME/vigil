#include "EventCollector.hpp"

#include "ProcessInfoReader.hpp"
#include "ProcessSnapshot.hpp"
#include "TcpTable.hpp"
#include "WinApi.hpp"

#include <vigil/Logger.hpp>

namespace vigil {

   // TODO: proper injection
   platform::windows::ProcessInfoReader processInfoReader;

   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::windows::EventCollector>();
   }

} // namespace vigil

namespace vigil::platform::windows {

   // Microsoft-Windows-Kernel-Process — available on Windows 8+.
   // Byte layout of GUID {0x22fb2cd6,0x0e7b,0x422b,{0xa0,0xc7,...}} in memory
   // (Data1/2/3 little-endian, Data4 verbatim).
   static constexpr std::array<uint8_t, 16> kKernelProcessProvider = {
       0xd6, 0x2c, 0xfb, 0x22, 0x7b, 0x0e, 0x2b, 0x42, 0xa0, 0xc7, 0x2f, 0xad, 0x1f, 0xd0, 0xe7, 0x16,
   };

   static constexpr USHORT kEventIdProcessStart = 1;
   static constexpr uint64_t kProcessKeyword = 0x10; // process-lifecycle only

   void EventCollector::start() {
      log::info("event collector starting");
      if (!init()) {
         log::error("event collector init failed");
         return;
      }
      running_ = true;
      log::info("event collector running");

      while (running_) {
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
   }

   void EventCollector::stop() {
      log::info("event collector stopping");
      running_ = false;

      stopEvent_.set();

      if (etwSession_)
         etwSession_->stop();

      if (etwThread_.joinable())
         etwThread_.join();
   }

   bool EventCollector::init() {
      auto event = Event::create(true, false);
      if (!event) {
         log::error("CreateEvent failed: {}", GetLastError());
         return false;
      }

      stopEvent_ = std::move(*event);

      const auto now = std::chrono::steady_clock::now();
      nextScanTime_ = now;
      nextNetScanTime_ = now;

      etwSession_ = EtwSession::start(L"vigil-kernel-process", kKernelProcessProvider, kProcessKeyword);
      if (!etwSession_) {
         log::warn("ETW unavailable (requires elevation); running in scan-only mode");
      } else {
         etwThread_ = std::thread([this] {
            etwSession_->consume([this](const _EVENT_RECORD& r) {
               if (r.EventHeader.EventDescriptor.Id != kEventIdProcessStart)
                  return;
               const auto pid = static_cast<uint32_t>(r.EventHeader.ProcessId);
               auto proc = processInfoReader.read(pid);
               if (!proc)
                  return;
               onProcess.emit(*proc);
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
         auto proc = processInfoReader.read(pid);
         if (!proc)
            return;
         onProcess.emit(*proc);
         ++count;
      });

      log::debug("process scan complete: {} processes", count);
   }

   void EventCollector::scanNetwork() {
      TcpTable::forEach([&](const TcpConnection& conn) {
         const auto key = std::to_string(conn.pid) + ':' + conn.remoteAddr + ':' + std::to_string(conn.remotePort);

         if (!seenConnections_.insert(key).second)
            return;

         auto proc = processInfoReader.read(conn.pid);
         if (!proc)
            return;

         proc->hasConnect = true;
         proc->connectDport = conn.remotePort;
         proc->connectDaddr = conn.remoteAddr;
         onProcess.emit(*proc);
      });
   }

} // namespace vigil::platform::windows
