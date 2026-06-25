#ifndef VIGIL_ALERT_SINK_HPP
#define VIGIL_ALERT_SINK_HPP

#include <memory>
#include <vector>

#include <vigil/Alert.hpp>
#include <vigil/Config.hpp>

namespace vigil {
/**
 * @brief Abstract output destination for generated alerts.
 *
 * Implementations serialize or forward alerts to a concrete backend such as
 * stdout, a JSON file, syslog, or another notification channel.
 */
class AlertSink {
public:
  /**
   * @brief Construct an alert sink base.
   */
  AlertSink() = default;

  /**
   * @brief Alert sinks own backend resources and cannot be copied.
   */
  AlertSink(const AlertSink &) = delete;

  /**
   * @brief Alert sinks own backend resources and cannot be copy-assigned.
   */
  AlertSink &operator=(const AlertSink &) = delete;

  /**
   * @brief Alert sinks are held by pointer and cannot be moved.
   */
  AlertSink(AlertSink &&) = delete;

  /**
   * @brief Alert sinks are held by pointer and cannot be move-assigned.
   */
  AlertSink &operator=(AlertSink &&) = delete;

  /**
   * @brief Release sink-specific resources.
   */
  virtual ~AlertSink() = default;

  /**
   * @brief Emit one alert to the concrete sink backend.
   *
   * @param alert Alert to serialize or forward.
   */
  virtual void emit(const Alert &alert) = 0;
};

/**
 * @brief Fan-out sink that forwards each alert to all configured sinks.
 */
class AlertDispatcher final {
public:
  /**
   * @brief Construct a dispatcher from already-created sinks.
   *
   * @param sinks Sink instances that will receive every emitted alert.
   */
  explicit AlertDispatcher(std::vector<std::unique_ptr<AlertSink>> sinks);

  /**
   * @brief Emit one alert to every configured sink.
   *
   * @param alert Alert to forward.
   */
  void emit(const Alert &alert);

private:
  std::vector<std::unique_ptr<AlertSink>> sinks_;
};

/**
 * @brief Create an alert dispatcher from parsed configuration.
 *
 * @param config Parsed application configuration.
 *
 * @return Dispatcher containing all enabled alert sinks.
 */
[[nodiscard]] std::unique_ptr<AlertDispatcher>
createAlertDispatcher(const Config &config);
} // namespace vigil

#endif // VIGIL_ALERT_SINK_HPP
