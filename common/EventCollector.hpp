
#ifndef VIGIL_COMMON_IEVENTCOLLECTOR_HPP
#define VIGIL_COMMON_IEVENTCOLLECTOR_HPP

#include <functional>
#include <memory>

#include "ProcessInfo.hpp"
#include "Signal.hpp"

namespace vigil::common {
    class EventCollector {
    public:
        Signal<const ProcessInfo &> onProcess;

        virtual void start() = 0;

        virtual void stop() = 0;

        virtual ~EventCollector() = default;
    };

    [[nodiscard]] std::unique_ptr<EventCollector> createEventCollector();
} // namespace vigil::common

#endif // VIGIL_COMMON_IEVENTCOLLECTOR_HPP
