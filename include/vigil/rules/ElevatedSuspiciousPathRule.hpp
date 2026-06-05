#ifndef VIGIL_RULES_ELEVATEDSUSPICIOUSPATHRULE_HPP
#define VIGIL_RULES_ELEVATEDSUSPICIOUSPATHRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
/**
 * @brief Detects elevated processes launched from configured suspicious paths.
 */
class ElevatedSuspiciousPathRule : public Rule {
public:
  /**
   * @brief Rule id used in configuration and alerts.
   */
  static constexpr std::string_view kName = "elevated_suspicious_path";

  /**
   * @brief Construct the rule from parsed configuration.
   *
   * @param cfg Parsed configuration for
   * this rule.
   */
  explicit ElevatedSuspiciousPathRule(RuleConfig cfg);

  [[nodiscard]] std::string_view name() const noexcept override;

protected:
  [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;

  [[nodiscard]] Alert makeAlert(const ProcessInfo &info) const override;
};
} // namespace vigil::rules

#endif // VIGIL_RULES_ELEVATEDSUSPICIOUSPATHRULE_HPP
