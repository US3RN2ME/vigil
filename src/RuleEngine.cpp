#include <vigil/Logger.hpp>
#include <vigil/RuleEngine.hpp>
#include <vigil/rules/AnonRwxRule.hpp>
#include <vigil/rules/PrivilegeEscalationRule.hpp>
#include <vigil/rules/ServerSpawnedShellRule.hpp>
#include <vigil/rules/SuspiciousPathRule.hpp>

namespace vigil {
   RuleEngine::RuleEngine(Config cfg)
       : cfg_{std::move(cfg)} {
      log::info("initializing rule engine");

      addRule(std::make_unique<rules::ServerSpawnedShellRule>(cfg_.rule(rules::ServerSpawnedShellRule::kName)));
      addRule(std::make_unique<rules::AnonRwxRule>(cfg_.rule(rules::AnonRwxRule::kName)));
      addRule(std::make_unique<rules::SuspiciousPathRule>(cfg_.rule(rules::SuspiciousPathRule::kName)));
      addRule(std::make_unique<rules::PrivilegeEscalationRule>(cfg_.rule(rules::PrivilegeEscalationRule::kName)));
   }

   void RuleEngine::process(const ProcessInfo& info) {
      for (auto& rule : rules_) {
         try {
            if (auto alert = rule->evaluate(info))
               onAlert.emit(std::move(*alert));
         } catch (const std::exception& e) {
            log::error("rule evaluation failed: {}", e.what());
         }
      }
   }

   void RuleEngine::addRule(std::unique_ptr<rules::Rule> rule) {
      if (!rule) {
         log::warn("attempted to add null rule");
         return;
      }
      rules_.emplace_back(std::move(rule));
   }
} // namespace vigil
