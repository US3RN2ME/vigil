#include <vigil/Error.hpp>
#include <vigil/EventCollector.hpp>
#include <vigil/Logger.hpp>
#include <vigil/RuleEngine.hpp>
#include <vigil/version.hpp>

int main() {
   try {
      vigil::log::init();

      vigil::log::info("vigil starting, version = {}", vigil::kVersion);

      auto collector = vigil::createEventCollector();
      auto engine = vigil::createRuleEngine(vigil::Config::loadFromFile(VIGIL_CONFIG_PATH));

      vigil::log::info("vigil started");

      collector->onProcess.connect([&](const vigil::ProcessInfo& info) {
         engine->process(info);
      });

      engine->onAlert.connect([](const vigil::Alert& alert) {
         std::string attrs;
         for (const auto& [k, v] : alert.attributes)
            attrs += std::format(" {}={}", k, v);

         vigil::log::warn("[ALERT] rule={} severity={} pid={} name={}{}", alert.rule, alert.severity, alert.info.pid,
                          alert.info.name, attrs);
      });

      collector->start();
      vigil::log::info("event collector stopped");

   } catch (const vigil::CollectorError& e) {
      vigil::log::error("Collector init failed: {}", e.what());
   } catch (const vigil::ConfigError& e) {
      vigil::log::error("Bad config: {}", e.what());
   } catch (...) {
      vigil::log::error("Unknown error");
   }
   return 0;
}
