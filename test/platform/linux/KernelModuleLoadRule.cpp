#include "rules/KernelModuleLoadRule.hpp"

#include "ut_main.hpp"

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
         const auto* event = attrValue(alert->attributes, "event");
         const auto* uid = attrValue(alert->attributes, "uid");
         expect(event != nullptr);
         expect(uid != nullptr);
         expect(eq(*event, std::string{"module_load"}));
         expect(eq(*uid, std::string{"1000"}));
      };
   };
} // namespace
