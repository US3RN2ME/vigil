
#ifndef VIGIL_EVENTCOLLECTOR_HPP
#define VIGIL_EVENTCOLLECTOR_HPP

#include <memory>

#include <vigil/ProcessInfo.hpp>
#include <vigil/Signal.hpp>

namespace vigil {
   class EventCollector {
   public:
      Signal<const ProcessInfo&> onProcess;

      virtual void start() = 0;
      virtual void stop() = 0;
      virtual ~EventCollector() = default;

      EventCollector(const EventCollector&) = delete;
      EventCollector& operator=(const EventCollector&) = delete;

   protected:
      EventCollector() = default;
   };

   std::unique_ptr<EventCollector> createEventCollector();
} // namespace vigil

#endif // VIGIL_EVENTCOLLECTOR_HPP
