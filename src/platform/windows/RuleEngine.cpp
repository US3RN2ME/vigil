
#include "rules/DebugPrivilegeRule.hpp"

#include <vigil/RuleEngine.hpp>

namespace vigil {
   std::unique_ptr<RuleEngine> createRuleEngine(Config cfg) {
      using namespace platform::windows::rules;
      auto engine = std::make_unique<RuleEngine>(std::move(cfg));
      engine->addRule(std::make_unique<DebugPrivilegeRule>(cfg.rule(DebugPrivilegeRule::kName)));
      return engine;
   }
} // namespace vigil
