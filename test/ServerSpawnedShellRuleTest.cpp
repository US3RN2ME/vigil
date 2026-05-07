#include "ut_main.hpp"

#include <vigil/rules/ServerSpawnedShellRuleTest.hpp>

namespace {
   suite<"[ServerSpawnedShellRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::RuleConfig;
      using vigil::rules::ServerSpawnedShellRule;

      const RuleConfig cfg{
          .serverNames = {"nginx", "apache"},
          .shellNames = {"bash", "sh"},
      };

      "[FiresWhenServerSpawnsShell]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "nginx";
         info.name = "bash";
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenParentNotServer]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "python";
         info.name = "bash";
         expect(!rule.evaluate(info).has_value());
      };

      "[SilentWhenChildNotShell]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "nginx";
         info.name = "python";
         expect(!rule.evaluate(info).has_value());
      };

      "[StripsExeSuffixFromParent]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "nginx.exe";
         info.name = "bash";
         expect(rule.evaluate(info).has_value());
      };

      "[StripsExeSuffixFromChild]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "nginx";
         info.name = "bash.exe";
         expect(rule.evaluate(info).has_value());
      };

      "[AlertContainsParentAndChild]"_test = [&] {
         ServerSpawnedShellRule rule{cfg};
         ProcessInfo info;
         info.parentName = "nginx";
         info.name = "bash";
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"nginx"}));
         expect(eq(alert->attributes[1].second, std::string{"bash"}));
      };
   };
} // namespace
