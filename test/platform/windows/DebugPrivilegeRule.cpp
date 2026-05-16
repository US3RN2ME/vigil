#include "rules/DebugPrivilegeRule.hpp"

#include "ut_main.hpp"

namespace {
   suite<"[DebugPrivilegeRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::rules::DebugPrivilegeRule;
      using vigil::rules::RuleConfig;

      constexpr uint64_t kSeDebugPrivilege = 1ULL << 20;

      "[FiresWhenSeDebugPrivilegeIsPresent]"_test = [] {
         DebugPrivilegeRule rule{RuleConfig{}};
         ProcessInfo info;
         info.privilegeMask = kSeDebugPrivilege;

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireWithoutSeDebugPrivilege]"_test = [] {
         DebugPrivilegeRule rule{RuleConfig{}};
         ProcessInfo info;
         info.privilegeMask = 1ULL << 19;

         expect(!rule.evaluate(info).has_value());
      };

      "[AlertContainsProcessContext]"_test = [] {
         DebugPrivilegeRule rule{RuleConfig{}};
         ProcessInfo info;
         info.privilegeMask = kSeDebugPrivilege;
         info.exePath = R"(C:\Windows\System32\cmd.exe)";
         info.cmdline = R"(cmd.exe /c whoami)";
         info.parentName = "powershell.exe";

         const auto alert = rule.evaluate(info);

         expect(alert.has_value());
         expect(eq(alert->attributes[0].second, info.exePath));
         expect(eq(alert->attributes[1].second, info.cmdline));
         expect(eq(alert->attributes[2].second, info.parentName));
      };
   };
} // namespace
