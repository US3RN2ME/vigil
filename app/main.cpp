#include <iostream>

#include "common/EventCollector.hpp"
#include "common/RuleEngine.hpp"

int main() {
    auto collector = vigil::common::createEventCollector();
    auto ruleEngine = vigil::common::createRuleEngine(vigil::common::Config::loadFromFile(VIGIL_CONFIG_PATH));

    collector->onProcess.connect([&ruleEngine](const vigil::common::ProcessInfo &info) {
        std::cout << "onProcess" << info.pid << " " << info.exePath << std::endl;
        ruleEngine->process(info);
    });

    ruleEngine->onAlert.connect([&](const vigil::common::Alert &alert) {
        std::cout << "onAlert" << alert.rule << " " << alert.severity << std::endl;
    });

    std::cout << "Starting..." << std::endl;

    collector->start();

    //collector->stop();

    return 0;
}
