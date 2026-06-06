#ifndef VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP
#define VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
/**
 * @brief Detects configured suspicious command-line substrings.
 */
class SuspiciousCmdLineRule : public Rule {
public:
  /**
   * @brief Rule id used in configuration and alerts.
   */
  static constexpr std::string_view kName = "suspicious_cmdline";

  /**
   * @brief Construct the rule from parsed configuration.
   *
   * @param cfg Parsed configuration for
   * this rule.
   */
  explicit SuspiciousCmdLineRule(RuleConfig cfg);

  [[nodiscard]] std::string_view name() const noexcept override;

protected:
  [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;

  [[nodiscard]] Alert makeAlert(const ProcessInfo &info) const override;

private:
  std::vector<std::string> lowerPatterns_;
};
} // namespace vigil::rules

#endif // VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP
