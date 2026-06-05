#include "rules/LdPreloadHijackRule.hpp"

#include "ut_main.hpp"

namespace {
suite<"[LdPreloadHijackRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::platform::rules::LdPreloadHijackRule;
  using vigil::rules::RuleConfig;

  "[FiresWhenLdPreloadSet]"_test = [] {
    LdPreloadHijackRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.hasLdPreload = true;
    expect(rule.evaluate(info).has_value());
  };

  "[AlertContainsPathAndCmdline]"_test = [] {
    LdPreloadHijackRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.hasLdPreload = true;
    info.exePath = "/usr/bin/curl";
    info.cmdline = "curl https://example.com";
    auto alert = rule.evaluate(info);
    const auto *env = attrValue(alert->attributes, "env");
    expect(env != nullptr);
    expect(eq(*env, std::string{"LD_PRELOAD"}));
  };
};
} // namespace
