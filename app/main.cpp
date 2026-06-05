#include <exception>
#include <thread>

#include <vigil/AlertSink.hpp>
#include <vigil/Error.hpp>
#include <vigil/EventCollector.hpp>
#include <vigil/Logger.hpp>
#include <vigil/RuleEngine.hpp>
#include <vigil/SignalHandler.hpp>
#include <vigil/Version.hpp>

int main() {
  try {
    auto logShutdownGuard = vigil::log::init();

    vigil::log::info("vigil {} starting...", vigil::kVersion);

    vigil::SignalHandler signalHandler;

    const auto config = vigil::Config::loadFromFile(VIGIL_CONFIG_PATH);
    auto collector = vigil::createEventCollector();
    auto engine = vigil::createRuleEngine(config);
    auto alerts = vigil::createAlertDispatcher(config);

    vigil::log::info("vigil started");

    collector->onProcess.connect(
        [&](const vigil::ProcessInfo &info) { engine->process(info); });

    engine->onAlert.connect(
        [&](const vigil::Alert &alert) { alerts->emit(alert); });

    std::exception_ptr collectorError;
    std::thread collectorThread([&] {
      try {
        collector->start();
      } catch (...) {
        collectorError = std::current_exception();
        signalHandler.requestStop();
      }
    });

    signalHandler.wait();

    vigil::log::info("Received signal '{}', stopping...",
                     vigil::toStringView(signalHandler.reason()));

    collector->stop();

    if (collectorThread.joinable()) {
      collectorThread.join();
    }

    if (collectorError)
      std::rethrow_exception(collectorError);

    vigil::waitForExitAcknowledgement();
  } catch (const vigil::SignalHandlerError &e) {
    vigil::log::error("Signal handler failed: {}", e.what());
    return 1;
  } catch (const vigil::CollectorError &e) {
    vigil::log::error("Collector failed: {}", e.what());
    return 1;
  } catch (const vigil::ConfigError &e) {
    vigil::log::error("Bad config: {}", e.what());
    return 1;
  } catch (...) {
    vigil::log::error("Unknown error");
    return 1;
  }
  return 0;
}
