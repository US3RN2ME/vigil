#include "ut_main.hpp"

#include <vigil/rules/FilelessExecutionRule.hpp>

namespace {
suite<"[FilelessExecutionRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::rules::FilelessExecutionRule;
  using vigil::rules::RuleConfig;

  "[FiresWhenExeDeleted]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.exeDeleted = true;

    expect(rule.evaluate(info).has_value());
  };

  "[FiresWhenMemfd]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.isMemfd = true;

    expect(rule.evaluate(info).has_value());
  };

  "[SilentWhenDisabled]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{.enabled = false}};
    ProcessInfo info;
    info.platform.isMemfd = true;

    expect(!rule.evaluate(info).has_value());
  };

  "[AlertAttributesDescribeDeletedExecutable]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.exeDeleted = true;
    info.exePath = "/tmp/payload (deleted)";

    const auto alert = rule.evaluate(info);

    expect(alert.has_value());
    const auto *deleted = attrValue(alert->attributes, "deleted");
    expect(deleted != nullptr);
    expect(eq(*deleted, std::string{"true"}));
  };
};
} // namespace
