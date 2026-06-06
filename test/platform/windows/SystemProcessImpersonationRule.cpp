#include "rules/SystemProcessImpersonationRule.hpp"

#include "ut_main.hpp"

namespace {
vigil::rules::RuleConfig config() {
  vigil::rules::RuleConfig cfg;
  cfg.protectedProcessNames.insert("lsass.exe");
  return cfg;
}

suite<"[SystemProcessImpersonationRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::platform::rules::SystemProcessImpersonationRule;
  using vigil::rules::RuleConfig;

  "[FiresForProtectedNameOutsideSystemDirectory]"_test = [] {
    SystemProcessImpersonationRule rule{config()};
    ProcessInfo info;
    info.name = "lsass.exe";
    info.exePath = R"(C:\Users\Public\lsass.exe)";

    expect(rule.evaluate(info).has_value());
  };

  "[DoesNotFireForProtectedNameInSystemDirectory]"_test = [] {
    SystemProcessImpersonationRule rule{config()};
    ProcessInfo info;
    info.name = "lsass.exe";
    info.exePath = R"(C:\Windows\System32\lsass.exe)";

    expect(!rule.evaluate(info).has_value());
  };

  "[DoesNotFireWhenNameIsNotConfigured]"_test = [] {
    SystemProcessImpersonationRule rule{RuleConfig{}};
    ProcessInfo info;
    info.name = "lsass.exe";
    info.exePath = R"(C:\Users\Public\lsass.exe)";

    expect(!rule.evaluate(info).has_value());
  };
};
} // namespace
