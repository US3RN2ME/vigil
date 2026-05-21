#include <vigil/Logger.hpp>
#include <vigil/RuleEngine.hpp>
#include <vigil/rules/AnonRwxRuleTest.hpp>
#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>
#include <vigil/rules/FilelessExecutionRule.hpp>
#include <vigil/rules/PrivilegeEscalationRuleTest.hpp>
#include <vigil/rules/ServerSpawnedShellRuleTest.hpp>
#include <vigil/rules/SuspiciousCmdLineRule.hpp>
#include <vigil/rules/SuspiciousPathRuleTest.hpp>
#include <vigil/rules/SuspiciousPortRule.hpp>

namespace vigil {
   RuleEngine::RuleEngine(Config cfg)
       : cfg_{std::move(cfg)} {
      log::info("initializing rule engine");

      addRule(std::make_unique<rules::ServerSpawnedShellRule>(cfg_.rule(rules::ServerSpawnedShellRule::kName)));
      addRule(std::make_unique<rules::AnonRwxRule>(cfg_.rule(rules::AnonRwxRule::kName)));
      addRule(std::make_unique<rules::SuspiciousPathRule>(cfg_.rule(rules::SuspiciousPathRule::kName)));
      addRule(std::make_unique<rules::ElevatedSuspiciousPathRule>(cfg_.rule(rules::ElevatedSuspiciousPathRule::kName)));
      addRule(std::make_unique<rules::FilelessExecutionRule>(cfg_.rule(rules::FilelessExecutionRule::kName)));
      addRule(std::make_unique<rules::PrivilegeEscalationRule>(cfg_.rule(rules::PrivilegeEscalationRule::kName)));
      addRule(std::make_unique<rules::SuspiciousCmdLineRule>(cfg_.rule(rules::SuspiciousCmdLineRule::kName)));
      addRule(std::make_unique<rules::SuspiciousPortRule>(cfg_.rule(rules::SuspiciousPortRule::kName)));
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
      log::debug("registered rule: {}", rule->name());
      rules_.emplace_back(std::move(rule));
   }
} // namespace vigil
