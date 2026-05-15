#include <thread>

#include <vigil/Error.hpp>
#include <vigil/EventCollector.hpp>
#include <vigil/Logger.hpp>
#include <vigil/RuleEngine.hpp>
#include <vigil/SignalHandler.hpp>
#include <vigil/Version.hpp>

int main() {
   try {
      vigil::log::init();

      vigil::log::info("vigil {} starting...", vigil::kVersion);

      vigil::SignalHandler signalHandler;

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

      std::thread collectorThread([&] {
         collector->start();
      });

      signalHandler.wait();

      vigil::log::info("Received signal '{}', stopping...", vigil::toStringView(signalHandler.reason()));

      collector->stop();

      if (collectorThread.joinable()) {
         collectorThread.join();
      }

      vigil::waitForExitAcknowledgement();
   } catch (const vigil::SignalHandlerError& e) {
      vigil::log::error("Signal handler failed: {}", e.what());
   } catch (const vigil::CollectorError& e) {
      vigil::log::error("Collector init failed: {}", e.what());
   } catch (const vigil::ConfigError& e) {
      vigil::log::error("Bad config: {}", e.what());
   } catch (...) {
      vigil::log::error("Unknown error");
   }
   return 0;
}
