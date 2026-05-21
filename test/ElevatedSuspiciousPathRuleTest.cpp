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
#if defined(__linux__)
         info.platform.euid = 0;
         info.exePath = "/tmp/payload";
#elif defined(_WIN32)
         info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::High;
         info.exePath = R"(C:\Users\Public\payload.exe)";
#endif

         expect(rule.evaluate(info).has_value());
      };

#if defined(_WIN32)
      "[FiresForHighIntegrityProcessInSuspiciousPath]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::High;
         info.exePath = R"(C:\Users\Public\payload.exe)";

         expect(rule.evaluate(info).has_value());
      };
#endif

      "[DoesNotFireWhenNotElevated]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
#if defined(__linux__)
         info.platform.euid = 1000;
         info.exePath = "/tmp/payload";
#elif defined(_WIN32)
         info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::Medium;
         info.exePath = R"(C:\Users\Public\payload.exe)";
#endif

         expect(!rule.evaluate(info).has_value());
      };

      "[DoesNotFireForTrustedPath]"_test = [&cfg] {
         ElevatedSuspiciousPathRule rule{cfg};
         ProcessInfo info;
#if defined(__linux__)
         info.platform.euid = 0;
         info.exePath = "/usr/bin/sudo";
#elif defined(_WIN32)
         info.platform.integrity = ProcessInfo::PlatformInfo::Integrity::High;
         info.exePath = R"(C:\Windows\System32\cmd.exe)";
#endif

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
