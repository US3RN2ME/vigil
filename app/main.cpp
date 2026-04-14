#include <iostream>

#include "common/IEventCollector.hpp"

int main() {
    auto collector = vigil::common::createEventCollector();

    collector->setProcessCallback([](const vigil::common::ProcessInfo& p) {
        std::cout << p.pid << " " << p.exePath << std::endl;
        // rule engine
    });

    collector->start();

    collector->stop();

    return 0;
}