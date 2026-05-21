#include "ut_main.hpp"

#include <vigil/rules/SuspiciousCmdLineRule.hpp>

namespace {
   suite<"[SuspiciousCmdLineRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::RuleConfig;
      using vigil::rules::SuspiciousCmdLineRule;

      const RuleConfig cfg{.cmdlinePatterns = {"EncodedCommand", "bash -i"}};

      "[MatchesCaseInsensitively]"_test = [&cfg] {
         SuspiciousCmdLineRule rule{cfg};
         ProcessInfo info;
         info.cmdline = "powershell.exe -encodedcommand AAAA";

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireWithoutPattern]"_test = [&cfg] {
         SuspiciousCmdLineRule rule{cfg};
         ProcessInfo info;
         info.cmdline = "powershell.exe -NoProfile";

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
