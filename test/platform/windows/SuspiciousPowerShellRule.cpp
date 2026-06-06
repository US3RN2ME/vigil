#include "rules/SuspiciousPowerShellRule.hpp"

#include "ut_main.hpp"

namespace {
suite<"[SuspiciousPowerShellRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::platform::rules::SuspiciousPowerShellRule;
  using vigil::rules::RuleConfig;

  "[FiresForEncodedCommand]"_test = [] {
    SuspiciousPowerShellRule rule{RuleConfig{}};
    ProcessInfo info;
    info.name = "powershell.exe";
    info.cmdline = "powershell.exe -NoProfile -EncodedCommand AAAA";

    expect(rule.evaluate(info).has_value());
  };

  "[DoesNotFireForNonPowerShell]"_test = [] {
    SuspiciousPowerShellRule rule{RuleConfig{}};
    ProcessInfo info;
    info.name = "cmd.exe";
    info.cmdline = "cmd.exe /c echo -EncodedCommand";

    expect(!rule.evaluate(info).has_value());
  };
};
} // namespace
