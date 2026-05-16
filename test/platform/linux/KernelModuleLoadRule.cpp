#include "rules/KernelModuleLoadRule.hpp"

#include "main.hpp"

namespace {
   suite<"[KernelModuleLoadRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::rules::KernelModuleLoadRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenModuleLoaded]"_test = [] {
         KernelModuleLoadRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.hasModuleLoad = true;
         expect(rule.evaluate(info).has_value());
      };

      "[AlertContainsPathCmdlineUid]"_test = [] {
         KernelModuleLoadRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.hasModuleLoad = true;
         info.exePath = "/usr/bin/insmod";
         info.cmdline = "insmod rootkit.ko";
         info.platform.uid = 1000;
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"/usr/bin/insmod"}));
         expect(eq(alert->attributes[1].second, std::string{"insmod rootkit.ko"}));
         expect(eq(alert->attributes[2].second, std::string{"1000"}));
      };
   };
} // namespace
