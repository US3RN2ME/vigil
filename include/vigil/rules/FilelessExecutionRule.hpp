#ifndef VIGIL_RULES_FILELESSEXECUTIONRULE_HPP
#define VIGIL_RULES_FILELESSEXECUTIONRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   /**
    * @brief Detects processes whose image is missing from disk or backed by memfd.
    */
   class FilelessExecutionRule : public Rule {
   public:
      /**
       * @brief Rule id used in configuration and alerts.
       */
      static constexpr std::string_view kName = "fileless_execution";

      /**
       * @brief Construct the rule from parsed configuration.
       *
       * @param cfg Parsed configuration for
       * this rule.
       */
      explicit FilelessExecutionRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_FILELESSEXECUTIONRULE_HPP
