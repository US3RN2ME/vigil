
#include "rules/BinaryReplacedRule.hpp"
#include "rules/FilelessExecutionRule.hpp"
#include "rules/KernelModuleLoadRule.hpp"
#include "rules/LdPreloadHijackRule.hpp"
#include "rules/NetworkConnectRule.hpp"
#include "rules/PrivilegeEscalationRule.hpp"
#include "rules/PtraceInjectRule.hpp"

#include <vigil/RuleEngine.hpp>

namespace vigil {
   std::unique_ptr<RuleEngine> createRuleEngine(Config cfg) {
      using namespace platform::linux::rules;
      auto engine = std::make_unique<RuleEngine>(std::move(cfg));
      engine->addRule(std::make_unique<BinaryReplacedRule>(cfg.rule(BinaryReplacedRule::kName)));
      engine->addRule(std::make_unique<FilelessExecutionRule>(cfg.rule(FilelessExecutionRule::kName)));
      engine->addRule(std::make_unique<LdPreloadHijackRule>(cfg.rule(LdPreloadHijackRule::kName)));
      engine->addRule(std::make_unique<KernelModuleLoadRule>(cfg.rule(KernelModuleLoadRule::kName)));
      engine->addRule(std::make_unique<NetworkConnectRule>(cfg.rule(NetworkConnectRule::kName)));
      engine->addRule(std::make_unique<PrivilegeEscalationRule>(cfg.rule(PrivilegeEscalationRule::kName)));
      engine->addRule(std::make_unique<PtraceInjectRule>(cfg.rule(PtraceInjectRule::kName)));
      return engine;
   }
} // namespace vigil
