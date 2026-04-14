
#ifndef VIGIL_COMMON_IEVENTCOLLECTOR_HPP
#define VIGIL_COMMON_IEVENTCOLLECTOR_HPP

#include <functional>
#include <memory>

#include "ProcessInfo.hpp"

namespace vigil::common {
    class IEventCollector {
    public:
        using ProcessCallback = std::function<void(const ProcessInfo&)>;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void setProcessCallback(ProcessCallback callback) {
            callback_ = std::move(callback);
        }

        virtual ~IEventCollector() = default;

    protected:
        ProcessCallback callback_;
    };

    [[nodiscard]] std::unique_ptr<IEventCollector> createEventCollector();
} // namespace vigil::common

#endif // VIGIL_COMMON_IEVENTCOLLECTOR_HPP
