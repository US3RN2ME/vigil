
#include "RuleEngine.hpp"

#include "rules/AnonRwxRule.hpp"
#include "rules/PrivilegeEscalationRule.hpp"
#include "rules/ServerSpawnedShellRule.hpp"
#include "rules/SuspiciousPathRule.hpp"

namespace vigil::common {
    RuleEngine::RuleEngine(Config cfg)
        : cfg_{std::move(cfg)} {
        addRule(std::make_unique<rules::ServerSpawnedShellRule>(cfg_.rule(rules::ServerSpawnedShellRule::kName)));
        addRule(std::make_unique<rules::AnonRwxRule>(cfg_.rule(rules::AnonRwxRule::kName)));
        addRule(std::make_unique<rules::SuspiciousPathRule>(cfg_.rule(rules::SuspiciousPathRule::kName)));
        addRule(std::make_unique<rules::PrivilegeEscalationRule>(cfg_.rule(rules::PrivilegeEscalationRule::kName)));
    }

    void RuleEngine::addRule(std::unique_ptr<rules::Rule> rule) {
        rules_.emplace_back(std::move(rule));
    }

    void RuleEngine::process(const ProcessInfo &info) {
        for (const auto &rule: rules_) {
            if (auto alert = rule->evaluate(info); alert) {
                onAlert.emit(std::move(*alert));
            }
        }
    }
}
