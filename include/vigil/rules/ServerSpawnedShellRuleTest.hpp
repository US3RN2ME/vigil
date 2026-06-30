
#ifndef VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP
#define VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP

#include <vigil/rules/Rule.hpp>
#include <vigil/rules/RuleConfig.hpp>

namespace vigil::rules {
   /**
    * @brief Detects configured server processes spawning interactive shells.
    */
   class ServerSpawnedShellRule : public Rule {
   public:
      /**
       * @brief Rule id used in configuration and alerts.
       */
      static constexpr std::string_view kName = "server_spawned_shell";

      /**
       * @brief Construct the rule from parsed configuration.
       *
       * @param cfg Parsed configuration for
       * this rule.
       */
      explicit ServerSpawnedShellRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP
