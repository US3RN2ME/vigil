#include "common/IEventCollector.hpp"

int main() {
    auto collector = vigil::common::createEventCollector();

    collector->setProcessCallback([](const vigil::common::ProcessInfo& p) {
        // rule engine
    });

    collector->start();
    // блокируется до сигнала
    collector->stop();

    return 0;
}