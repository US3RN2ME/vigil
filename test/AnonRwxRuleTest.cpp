#include "ut_main.hpp"

#include <vigil/rules/AnonRwxRuleTest.hpp>

namespace {
suite<"[AnonRwxRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::rules::AnonRwxRule;
  using vigil::rules::RuleConfig;

  "[FiresWhenFlagSet]"_test = [] {
    AnonRwxRule rule{RuleConfig{}};
    ProcessInfo info;
    info.hasAnonRwx = true;
    expect(rule.evaluate(info).has_value());
  };

  "[AlertContainsMemoryEvidence]"_test = [] {
    AnonRwxRule rule{RuleConfig{}};
    ProcessInfo info;
    info.hasAnonRwx = true;

    const auto alert = rule.evaluate(info);

    expect(alert.has_value());
    const auto *memory = attrValue(alert->attributes, "memory");
    expect(memory != nullptr);
    expect(eq(*memory, std::string{"anonymous_rwx"}));
  };

  "[SilentWhenDisabled]"_test = [] {
    AnonRwxRule rule{RuleConfig{.enabled = false}};
    ProcessInfo info;
    info.hasAnonRwx = true;
    expect(!rule.evaluate(info).has_value());
  };
};
} // namespace
