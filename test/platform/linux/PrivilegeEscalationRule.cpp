#include "rules/PrivilegeEscalationRule.hpp"

#include "ut_main.hpp"

namespace {
suite<"[LinuxPrivilegeEscalationRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::platform::rules::PrivilegeEscalationRule;
  using vigil::rules::RuleConfig;

  "[FiresWhenSetuidToRootObserved]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.hasSetuidToRoot = true;

    expect(rule.evaluate(info).has_value());
  };

  "[DoesNotFireWithoutSetuidToRoot]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;

    expect(!rule.evaluate(info).has_value());
  };

  "[DoesNotFireForExcludedSetuidPath]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{.excludePaths = {"/usr/bin/sudo"}}};
    ProcessInfo info;
    info.platform.hasSetuidToRoot = true;
    info.exePath = "/usr/bin/sudo";

    expect(!rule.evaluate(info).has_value());
  };

  "[AlertContainsIdentityContext]"_test = [] {
    PrivilegeEscalationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.hasSetuidToRoot = true;
    info.exePath = "/usr/bin/sudo";
    info.cmdline = "sudo id";
    info.platform.uid = 1000;
    info.platform.euid = 0;

    const auto alert = rule.evaluate(info);

    expect(alert.has_value());
    const auto *uid = attrValue(alert->attributes, "uid");
    const auto *euid = attrValue(alert->attributes, "euid");
    expect(uid != nullptr);
    expect(euid != nullptr);
    expect(eq(*uid, std::string{"1000"}));
    expect(eq(*euid, std::string{"0"}));
  };
};
} // namespace
