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

      "[SilentWhenDisabled]"_test = [] {
         AnonRwxRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
         info.hasAnonRwx = true;
         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace