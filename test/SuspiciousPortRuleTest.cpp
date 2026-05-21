#include "ut_main.hpp"

#include <vigil/rules/SuspiciousPortRule.hpp>

namespace {
   suite<"[SuspiciousPortRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::RuleConfig;
      using vigil::rules::SuspiciousPortRule;

      const RuleConfig cfg{.suspiciousPorts = {4444, 9001}};

      "[FiresForConfiguredPort]"_test = [&cfg] {
         SuspiciousPortRule rule{cfg};
         ProcessInfo info;
         info.hasConnect = true;
         info.connectDport = 4444;

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireForUnconfiguredPort]"_test = [&cfg] {
         SuspiciousPortRule rule{cfg};
         ProcessInfo info;
         info.hasConnect = true;
         info.connectDport = 443;

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
