#include "EventCollector.hpp"

#include "ProcessInfoReader.hpp"
#include "vigil/Logger.hpp"

namespace vigil {
   // TODO: proper injection
   platform::windows::ProcessInfoReader processInfoReader;

   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::windows::EventCollector>();
   }
} // namespace vigil

namespace vigil::platform::windows {
   void EventCollector::start() {
      log::info("event collector starting");
      if (!init()) {
         log::error("event collector init failed, eBPF unavailable");
         return;
      }
      running_ = true;
      log::info("event collector running");

      while (running_) {

      }
   }

   void EventCollector::stop() {
      log::info("event collector stopping");
      running_ = false;
   }

   bool EventCollector::init() {

   }

} // namespace vigil::platform::windows
