#include "ut_main.hpp"

#include <vigil/rules/FilelessExecutionRule.hpp>

namespace {
suite<"[FilelessExecutionRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::rules::FilelessExecutionRule;
  using vigil::rules::RuleConfig;

  "[FiresWhenImageMissingFromDisk]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{}};
    ProcessInfo info;
    info.imageMissingFromDisk = true;

    expect(rule.evaluate(info).has_value());
  };

  "[SilentWhenDisabled]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{.enabled = false}};
    ProcessInfo info;
    info.imageMissingFromDisk = true;

    expect(!rule.evaluate(info).has_value());
  };

  "[AlertAttributesDescribeImageBacking]"_test = [] {
    FilelessExecutionRule rule{RuleConfig{}};
    ProcessInfo info;
    info.imageMissingFromDisk = true;
    info.exePath = "/tmp/payload (deleted)";

    const auto alert = rule.evaluate(info);

    expect(alert.has_value());
    expect(attrValue(alert->attributes, "deleted") == nullptr);
    const auto *missing = attrValue(alert->attributes, "missing_from_disk");
    expect(missing != nullptr);
    expect(eq(*missing, std::string{"true"}));
  };
};
} // namespace
