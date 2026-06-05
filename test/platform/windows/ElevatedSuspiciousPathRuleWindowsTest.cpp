#include "ut_main.hpp"

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace {
suite<"[ElevatedSuspiciousPathRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::rules::ElevatedSuspiciousPathRule;
  using vigil::rules::RuleConfig;

  const RuleConfig cfg{.suspiciousPaths = {R"(C:\Users\Public\)"}};

  "[FiresForElevatedProcessInSuspiciousPath]"_test = [&cfg] {
    ElevatedSuspiciousPathRule rule{cfg};
    ProcessInfo info;
    info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::High;
    info.exePath = R"(C:\Users\Public\payload.exe)";

    expect(rule.evaluate(info).has_value());
  };

  "[DoesNotFireWhenNotElevated]"_test = [&cfg] {
    ElevatedSuspiciousPathRule rule{cfg};
    ProcessInfo info;
    info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::Medium;
    info.exePath = R"(C:\Users\Public\payload.exe)";

    expect(!rule.evaluate(info).has_value());
  };

  "[DoesNotFireForTrustedPath]"_test = [&cfg] {
    ElevatedSuspiciousPathRule rule{cfg};
    ProcessInfo info;
    info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::High;
    info.exePath = R"(C:\Windows\System32\cmd.exe)";

    expect(!rule.evaluate(info).has_value());
  };
};
} // namespace
