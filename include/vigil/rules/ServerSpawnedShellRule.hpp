
#ifndef VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP
#define VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP

#include <vigil/rules/Rule.hpp>
#include <vigil/rules/RuleConfig.hpp>

namespace vigil::rules {
   class ServerSpawnedShellRule : public Rule {
  public:
      static constexpr std::string_view kName = "server_spawned_shell";

      explicit ServerSpawnedShellRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

  protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_SERVERSPAWNEDSHELLRULE_HPP
