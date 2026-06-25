
#ifndef VIGIL_CONFIG_HPP
#define VIGIL_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <vigil/rules/RuleConfig.hpp>

namespace vigil {
/**
 * @brief Supported alert output sink backends.
 */
enum class AlertSinkType {
  /**
   * @brief Write alerts to standard output.
   */
  Stdout,

  /**
   * @brief Append alerts to a JSON file.
   */
  JsonFile,

  /**
   * @brief Send alerts to the system syslog facility.
   */
  Syslog,
};

/**
 * @brief Wire formats supported by the syslog alert sink.
 */
enum class SyslogAlertFormat {
  /**
   * @brief Common Event Format.
   */
  Cef,

  /**
   * @brief Log Event Extended Format.
   */
  Leef,
};

/**
 * @brief Parsed configuration for one alert output sink.
 */
struct AlertSinkConfig {
  /**
   * @brief Sink backend type.
   */
  AlertSinkType type{AlertSinkType::Stdout};

  /**
   * @brief Whether this sink should be created.
   */
  bool enabled{true};

  /**
   * @brief Sink-specific filesystem path, such as the JSON output file.
   */
  std::string path{};

  /**
   * @brief Syslog serialization format.
   */
  SyslogAlertFormat format{SyslogAlertFormat::Cef};
};

/**
 * @brief Parsed rule configuration loaded from a platform JSON config file.
 */
class Config {
public:
  /**
   * @brief Load and parse a JSON configuration file.
   *
   * @param path Path to the JSON configuration
   * file.
   *
   * @return Parsed configuration object.
   *
   * @throws ConfigError when the file cannot
   * be opened or parsed.
   */
  [[nodiscard]] static Config loadFromFile(std::string_view path);

  /**
   * @brief Return the configuration for a rule id.
   *
   * Unknown rule ids return a disabled RuleConfig so callers can safely
   * construct optional platform rules
   * from a shared code path.
   *
   * @param name Stable rule identifier.
   *
   * @return Parsed
   * configuration for the rule, or a disabled config when absent.
   */
  [[nodiscard]] rules::RuleConfig rule(std::string_view name) const noexcept;

  /**
   * @brief Return configured alert sinks.
   */
  [[nodiscard]] const std::vector<AlertSinkConfig> &alertSinks() const noexcept;

private:
  std::unordered_map<std::string, rules::RuleConfig> rules_;
  std::vector<AlertSinkConfig> alertSinks_{AlertSinkConfig{}};
};
} // namespace vigil

#endif // VIGIL_CONFIG_HPP
