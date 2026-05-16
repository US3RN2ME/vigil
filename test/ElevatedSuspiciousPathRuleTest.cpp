#include "ut_main.hpp"

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace {
   suite<"[ElevatedSuspiciousPathRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::ElevatedSuspiciousPathRule;
      using vigil::rules::RuleConfig;

      const RuleConfig cfg{.suspiciousPaths = {"/tmp/", R"(C:\Users\Public\)"}};

      "[FiresForRootProcessInSuspiciousPath]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.euid = 0;
         info.exePath = "/tmp/payload";

         expect(rule.evaluate(info).has_value());
      };

      "[FiresForHighIntegrityProcessInSuspiciousPath]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.integrity = ProcessInfo::Integrity::High;
         info.exePath = R"(C:\Users\Public\payload.exe)";

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireWhenNotElevated]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.euid = 1000;
         info.integrity = ProcessInfo::Integrity::Medium;
         info.exePath = "/tmp/payload";

         expect(!rule.evaluate(info).has_value());
      };

      "[DoesNotFireForTrustedPath]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.euid = 0;
         info.exePath = "/usr/bin/sudo";

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
