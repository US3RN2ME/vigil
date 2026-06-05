
#include "rules/DebugPrivilegeRule.hpp"
#include "rules/LolbinExecutionRule.hpp"
#include "rules/SuspiciousPowerShellRule.hpp"
#include "rules/SystemProcessImpersonationRule.hpp"

#include <vigil/RuleEngine.hpp>

namespace vigil {
std::unique_ptr<RuleEngine> createRuleEngine(Config cfg) {
  using namespace platform::rules;
  auto engine = std::make_unique<RuleEngine>(cfg);
  engine->addRule(std::make_unique<DebugPrivilegeRule>(
      cfg.rule(DebugPrivilegeRule::kName)));
  engine->addRule(std::make_unique<SystemProcessImpersonationRule>(
      cfg.rule(SystemProcessImpersonationRule::kName)));
  engine->addRule(std::make_unique<LolbinExecutionRule>(
      cfg.rule(LolbinExecutionRule::kName)));
  engine->addRule(std::make_unique<SuspiciousPowerShellRule>(
      cfg.rule(SuspiciousPowerShellRule::kName)));
  return engine;
}
} // namespace vigil
