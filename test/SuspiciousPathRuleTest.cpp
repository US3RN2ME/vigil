#include "ut_main.hpp"

#include <vigil/rules/SuspiciousPathRuleTest.hpp>

namespace {
   suite<"[SuspiciousPathRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::RuleConfig;
      using vigil::rules::SuspiciousPathRule;

      const RuleConfig cfg{.suspiciousPaths = {"/tmp/", "/dev/shm/"}};

      "[FiresOnMatchingPrefix]"_test = [&] {
         SuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.exePath = "/tmp/malware";
         expect(rule.evaluate(info).has_value());
      };

      "[FiresOnSecondPrefix]"_test = [&] {
         SuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.exePath = "/dev/shm/payload";
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenNoMatchingPrefix]"_test = [&] {
         SuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.exePath = "/usr/bin/ls";
         expect(!rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [&] {
         SuspiciousPathRule rule{RuleConfig{.enabled = false, .suspiciousPaths = {"/tmp/"}}};
         ProcessInfo info;
         info.exePath = "/tmp/malware";
         expect(!rule.evaluate(info).has_value());
      };

      "[AlertContainsPath]"_test = [&] {
         SuspiciousPathRule rule{cfg};
         ProcessInfo info;
         info.exePath = "/tmp/malware";
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"/tmp/malware"}));
      };
   };
} // namespace