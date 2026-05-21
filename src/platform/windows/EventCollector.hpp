
#ifndef VIGIL_PLATFORM_EVENTCOLLECTOR_HPP
#define VIGIL_PLATFORM_EVENTCOLLECTOR_HPP

#include <chrono>
#include <thread>
#include <unordered_set>

#include "EtwSession.hpp"
#include "Event.hpp"

#include <vigil/EventCollector.hpp>
#include <vigil/ProcessInfoReader.hpp>

namespace vigil::platform {
   class EventCollector : public vigil::EventCollector {
      static constexpr std::chrono::seconds kScanInterval{30};
      static constexpr std::chrono::seconds kNetScanInterval{5};

   public:
      explicit EventCollector(std::unique_ptr<vigil::ProcessInfoReader> reader);

      void start() override;
      void stop() override;

   private:
      bool init();
      void scanProcesses();
      void scanNetwork();

      std::optional<EtwSession> etwSession_;
      std::thread etwThread_;
      std::atomic<bool> running_{false};

      Event stopEvent_;

      std::chrono::steady_clock::time_point nextScanTime_;
      std::chrono::steady_clock::time_point nextNetScanTime_;

      std::unordered_set<std::string> seenConnections_;
   };
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_EVENTCOLLECTOR_HPP
