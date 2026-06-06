#ifndef VIGIL_ALERT_SINK_HPP
#define VIGIL_ALERT_SINK_HPP

#include <memory>
#include <vector>

#include <vigil/Alert.hpp>
#include <vigil/Config.hpp>

namespace vigil {
class AlertSink {
public:
  AlertSink() = default;
  AlertSink(const AlertSink &) = delete;
  AlertSink &operator=(const AlertSink &) = delete;
  AlertSink(AlertSink &&) = delete;
  AlertSink &operator=(AlertSink &&) = delete;
  virtual ~AlertSink() = default;

  virtual void emit(const Alert &alert) = 0;
};

class AlertDispatcher final {
public:
  explicit AlertDispatcher(std::vector<std::unique_ptr<AlertSink>> sinks);

  void emit(const Alert &alert);

private:
  std::vector<std::unique_ptr<AlertSink>> sinks_;
};

[[nodiscard]] std::unique_ptr<AlertDispatcher>
createAlertDispatcher(const Config &config);
} // namespace vigil

#endif // VIGIL_ALERT_SINK_HPP
