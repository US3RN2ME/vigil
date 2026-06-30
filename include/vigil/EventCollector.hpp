
#ifndef VIGIL_EVENTCOLLECTOR_HPP
#define VIGIL_EVENTCOLLECTOR_HPP

#include <memory>

#include <vigil/ProcessInfo.hpp>
#include <vigil/ProcessInfoReader.hpp>
#include <vigil/Signal.hpp>

namespace vigil {
   /**
    * @brief Platform event source that emits normalized process snapshots.
    */
   class EventCollector {
   public:
      /**
       * @brief Emitted whenever the collector observes or scans a process.
       */
      Signal<const ProcessInfo&> onProcess;

      /**
       * @brief Start collecting events. Implementations may block until stop() is
       * called.
       *
       * @throws
       * CollectorError if the platform collector cannot start.
       */
      virtual void start() = 0;

      /**
       * @brief Request collector shutdown.
       */
      virtual void stop() = 0;

      virtual ~EventCollector() = default;

      EventCollector(const EventCollector&) = delete;
      EventCollector& operator=(const EventCollector&) = delete;

   protected:
      EventCollector() = default;

      /**
       * @brief Construct a collector with an explicit process snapshot reader.
       *
       * @param reader
       * Process metadata reader used by the collector.
       */
      explicit EventCollector(std::unique_ptr<ProcessInfoReader> reader);

      /**
       * @brief Reader used to enrich event callbacks with full process metadata.
       */
      std::unique_ptr<ProcessInfoReader> processInfoReader_;
   };

   /**
    * @brief Create the platform-specific event collector for the current build
    * target.
    *
    * @return Collector
    * implementation for the configured platform.
    */
   std::unique_ptr<EventCollector> createEventCollector();
} // namespace vigil

#endif // VIGIL_EVENTCOLLECTOR_HPP
