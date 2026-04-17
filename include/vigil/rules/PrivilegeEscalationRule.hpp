
#ifndef VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP
#define VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP

#include <unordered_map>

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   class PrivilegeEscalationRule : public Rule {
  public:
      static constexpr std::string_view kName = "privilege_escalation";

      explicit PrivilegeEscalationRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

  protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

  private:
      std::unordered_map<uint32_t, uint64_t> baseline_;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP
