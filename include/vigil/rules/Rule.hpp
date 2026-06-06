
#ifndef VIGIL_RULES_RULE_HPP
#define VIGIL_RULES_RULE_HPP

#include <optional>

#include <vigil/Alert.hpp>
#include <vigil/ProcessInfo.hpp>
#include <vigil/rules/RuleConfig.hpp>

namespace vigil::rules {
/**
 * @brief Base class for all detection rules.
 */
class Rule {
public:
  /**
   * @brief Construct a rule with its parsed configuration.
   *
   * @param cfg Parsed configuration for
   * this rule.
   */
  explicit Rule(RuleConfig cfg);

  virtual ~Rule() = default;

  /**
   * @brief Evaluate a process snapshot if the rule is enabled.
   *
   * @param info Process snapshot to
   * evaluate.
   *
   * @return an alert when the rule fires, otherwise std::nullopt.
   */
  [[nodiscard]] std::optional<Alert> evaluate(const ProcessInfo &info);

  /**
   * @brief Stable rule identifier used in configuration and alerts.
   */
  [[nodiscard]] virtual std::string_view name() const noexcept = 0;

protected:
  /**
   * @brief Rule-specific predicate and alert creation hook.
   *
   * @param info Process snapshot to
   * evaluate.
   *
   * @return an alert when the rule fires, otherwise std::nullopt.
   */
  [[nodiscard]] virtual std::optional<Alert> check(const ProcessInfo &info) = 0;

  /**
   * @brief Build a default alert for this rule and process.
   *
   * @param info Process snapshot that
   * triggered the alert.
   *
   * @return Alert populated with this rule's name, severity, and process info.
   */
  [[nodiscard]] virtual Alert makeAlert(const ProcessInfo &info) const;

  /**
   * @brief Parsed configuration for this rule.
   */
  const RuleConfig cfg_;
};
} // namespace vigil::rules

#endif // VIGIL_RULES_RULE_HPP
