#include <iostream>

#include <vigil/EventCollector.hpp>
#include <vigil/RuleEngine.hpp>

int main() {
   auto collector = vigil::createEventCollector();
   auto engine = vigil::createRuleEngine(vigil::Config::loadFromFile(VIGIL_CONFIG_PATH));

   collector->onProcess.connect([&](const vigil::ProcessInfo& info) {
      engine->process(info);
   });

   engine->onAlert.connect([](const vigil::Alert& alert) {
      std::cout << alert.rule << " " << alert.severity << "\n";
   });

   collector->start();
   return 0;
}