#include "rules/PtraceInjectRule.hpp"

#include "main.hpp"

namespace {
   suite<"[PtraceInjectRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::linux::rules::PtraceInjectRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenPtraceAttach]"_test = [] {
         PtraceInjectRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasPtraceAttach = true;
         expect(rule.evaluate(info).has_value());
      };

      "[AlertContainsPathCmdlineAndTargetPid]"_test = [] {
         PtraceInjectRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasPtraceAttach = true;
         info.exePath = "/usr/bin/gdb";
         info.cmdline = "gdb -p 1234";
         info.ptraceTargetPid = 1234;
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"/usr/bin/gdb"}));
         expect(eq(alert->attributes[1].second, std::string{"gdb -p 1234"}));
         expect(eq(alert->attributes[2].second, std::string{"1234"}));
      };
   };
} // namespace
