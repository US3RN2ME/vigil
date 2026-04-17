
#ifndef VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
#define VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP

#include <vigil/EventCollector.hpp>

namespace vigil::platform::windows {
   class EventCollector : public vigil::EventCollector {
   public:
      void start() override;
      void stop() override;
   };
} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_LINUX_EVENTCOLLECTOR_HPP
