#include "rules/DangerousCapabilitiesRule.hpp"

#include "ut_main.hpp"

namespace {
suite<"[DangerousCapabilitiesRule]"> _ = [] {
  using vigil::ProcessInfo;
  using vigil::platform::rules::DangerousCapabilitiesRule;
  using vigil::rules::RuleConfig;

  "[FiresForNonRootWithSysAdminCapability]"_test = [] {
    DangerousCapabilitiesRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.euid = 1000;
    info.privilegeMask = 1ULL << 21;

    expect(rule.evaluate(info).has_value());
  };

  "[DoesNotFireForRoot]"_test = [] {
    DangerousCapabilitiesRule rule{RuleConfig{}};
    ProcessInfo info;
    info.platform.euid = 0;
    info.privilegeMask = 1ULL << 21;

    expect(!rule.evaluate(info).has_value());
  };
};
} // namespace
