
#ifndef VIGIL_CONFIG_HPP
#define VIGIL_CONFIG_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <vigil/rules/RuleConfig.hpp>

namespace vigil {
enum class AlertSinkType {
  Stdout,
  JsonFile,
  Syslog,
};

enum class SyslogAlertFormat {
  Cef,
  Leef,
};

struct AlertSinkConfig {
  AlertSinkType type{AlertSinkType::Stdout};
  bool enabled{true};
  std::string path{};
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
