
#ifndef VIGIL_PLATFORM_RULES_PRIVILEGEESCALATIONRULE_HPP
#define VIGIL_PLATFORM_RULES_PRIVILEGEESCALATIONRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::rules {
class PrivilegeEscalationRule : public vigil::rules::Rule {
public:
  static constexpr std::string_view kName = "privilege_escalation_rule";

  explicit PrivilegeEscalationRule(vigil::rules::RuleConfig cfg);

  [[nodiscard]] std::string_view name() const noexcept override;

protected:
  [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;

  [[nodiscard]] Alert makeAlert(const ProcessInfo &info) const override;
};
} // namespace vigil::platform::rules

#endif // VIGIL_PLATFORM_RULES_PRIVILEGEESCALATIONRULE_HPP
