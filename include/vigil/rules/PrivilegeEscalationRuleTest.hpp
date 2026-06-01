
#ifndef VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP
#define VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP

#include <unordered_map>

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   /**
    * @brief Detects privilege mask increases for a process across snapshots.
    */
   class PrivilegeEscalationRule : public Rule {
   public:
      /**
       * @brief Rule id used in configuration and alerts.
       */
      static constexpr std::string_view kName = "privilege_escalation";

      /**
       * @brief Construct the rule from parsed configuration.
       *
       * @param cfg Parsed configuration for
       * this rule.
       */
      explicit PrivilegeEscalationRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;

   private:
      struct Baseline {
         uint64_t startTimeNs;
         uint64_t privilegeMask;
      };

      std::unordered_map<uint32_t, Baseline> baseline_;
      uint64_t alertBaseline_ = 0;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_PRIVILEGEESCALATIONRULE_HPP
