
#include "ut_main.hpp"

#include <vigil/rules/PrivilegeEscalationRuleTest.hpp>

namespace {
   suite<"[PrivilegeEscalationRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::PrivilegeEscalationRule;
      using vigil::rules::RuleConfig;

      "[SilentOnFirstObservation]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo info;
         info.pid = 1;
         info.privilegeMask = 0x1;
         expect(!rule.evaluate(info).has_value());
      };

      "[FiresWhenMaskIncreases]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo info;
         info.pid = 1;
         info.privilegeMask = 0x1;
         rule.evaluate(info);
         info.privilegeMask = 0x3;
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenMaskUnchanged]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo info;
         info.pid = 1;
         info.privilegeMask = 0x3;
         rule.evaluate(info);
         expect(!rule.evaluate(info).has_value());
      };

      "[SilentWhenMaskDecreases]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo info;
         info.pid = 1;
         info.privilegeMask = 0x3;
         rule.evaluate(info);
         info.privilegeMask = 0x1;
         expect(!rule.evaluate(info).has_value());
      };

      "[BaselineUpdatedAfterEscalation]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo info;
         info.pid = 1;
         info.privilegeMask = 0x1;
         rule.evaluate(info);
         info.privilegeMask = 0x3;
         rule.evaluate(info);
         expect(!rule.evaluate(info).has_value());
      };

      "[TracksPerPidIndependently]"_test = [] {
         PrivilegeEscalationRule rule{RuleConfig{}};
         ProcessInfo a;
         a.pid = 1;
         a.privilegeMask = 0x1;
         ProcessInfo b;
         b.pid = 2;
         b.privilegeMask = 0x1;
         rule.evaluate(a);
         rule.evaluate(b);
         a.privilegeMask = 0x3;
         expect(rule.evaluate(a).has_value());
         expect(!rule.evaluate(b).has_value());
      };
   };
} // namespace
