#include "EventCollector.hpp"

namespace vigil {
   std::unique_ptr<EventCollector> createEventCollector() {
      return std::make_unique<platform::windows::EventCollector>();
   }
} // namespace vigil

namespace vigil::platform::windows {
   void EventCollector::start() {
      // TODO: ETW
   }

   void EventCollector::stop() {
      // TODO
   }
} // namespace vigil::platform::windows