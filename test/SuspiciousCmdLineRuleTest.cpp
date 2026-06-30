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

      "[DoesNotFireForExcludedExactPath]"_test = [] {
         SuspiciousCmdLineRule rule{
             RuleConfig{.cmdlinePatterns = {"EncodedCommand"}, .excludePaths = {R"(C:\Program Files\Vendor\backup.ps1)"}}};
         ProcessInfo info;
         info.exePath = R"(C:\Program Files\Vendor\backup.ps1)";
         info.cmdline = "powershell.exe -EncodedCommand AAAA";

         expect(!rule.evaluate(info).has_value());
      };

      "[DoesNotFireForExcludedPathPrefix]"_test = [] {
         SuspiciousCmdLineRule rule{
             RuleConfig{.cmdlinePatterns = {"EncodedCommand"}, .excludePaths = {R"(C:\Program Files\Vendor\)"}}};
         ProcessInfo info;
         info.exePath = R"(C:\Program Files\Vendor\backup.ps1)";
         info.cmdline = "powershell.exe -EncodedCommand AAAA";

         expect(!rule.evaluate(info).has_value());
      };

      "[DoesNotFireForExcludedParent]"_test = [] {
         SuspiciousCmdLineRule rule{
             RuleConfig{.cmdlinePatterns = {"EncodedCommand"}, .excludeParentNames = {"backup-agent.exe"}}};
         ProcessInfo info;
         info.parentName = "backup-agent.exe";
         info.cmdline = "powershell.exe -EncodedCommand AAAA";

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
