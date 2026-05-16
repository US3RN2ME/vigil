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
         expect(eq(alert->attributes[0].second, info.exePath));
         expect(eq(alert->attributes[1].second, info.cmdline));
         expect(eq(alert->attributes[2].second, std::string{"1000"}));
         expect(eq(alert->attributes[3].second, std::string{"0"}));
      };
   };
} // namespace
