
#include <tuple>

#include "ut_main.hpp"

#include <vigil/rules/PrivilegeEscalationRuleTest.hpp>

namespace {
suite<"[PrivilegeEscalationRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::rules::PrivilegeEscalationRule;
  using vigil::rules::RuleConfig;

  "[SilentOnFirstObservation]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x1;
    expect(!rule.evaluate(info).has_value());
  };

  "[FiresWhenMaskIncreases]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x1;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x3;
    expect(rule.evaluate(info).has_value());
  };

  "[SilentWhenMaskUnchanged]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x3;
    std::ignore = rule.evaluate(info);
    expect(!rule.evaluate(info).has_value());
  };

  "[SilentWhenMaskDecreases]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x3;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x1;
    expect(!rule.evaluate(info).has_value());
  };

  "[BaselineUpdatedAfterEscalation]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x1;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x3;
    std::ignore = rule.evaluate(info);
    expect(!rule.evaluate(info).has_value());
  };

  "[FiresAfterMaskDropsAndRegainsPrivileges]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x3;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x1;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x3;
    expect(rule.evaluate(info).has_value());
  };

  "[SilentWhenPidIsReused]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.startTimeNs = 1;
    info.privilegeMask = 0x1;
    std::ignore = rule.evaluate(info);
    info.startTimeNs = 2;
    info.privilegeMask = 0x3;
    expect(!rule.evaluate(info).has_value());
  };

  "[AlertContainsPreviousAndCurrentMask]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.pid = 1;
    info.privilegeMask = 0x1;
    std::ignore = rule.evaluate(info);
    info.privilegeMask = 0x3;
    const auto alert = rule.evaluate(info);
    expect(alert.has_value());
    const auto *before = attrValue(alert->attributes, "mask_before");
    const auto *after = attrValue(alert->attributes, "mask_after");
    expect(before != nullptr);
    expect(after != nullptr);
    expect(eq(*before, std::string{"0x1"}));
    expect(eq(*after, std::string{"0x3"}));
  };

  "[TracksPerPidIndependently]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo a;
    a.pid = 1;
    a.privilegeMask = 0x1;
    ProcessInfo b;
    b.pid = 2;
    b.privilegeMask = 0x1;
    std::ignore = rule.evaluate(a);
    std::ignore = rule.evaluate(b);
    a.privilegeMask = 0x3;
    expect(rule.evaluate(a).has_value());
    expect(!rule.evaluate(b).has_value());
  };
};
} // namespace
