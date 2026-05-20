#include "rules/BinaryReplacedRule.hpp"

#include "ut_main.hpp"

namespace {
   suite<"[BinaryReplacedRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::rules::BinaryReplacedRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenBinaryReplaced]"_test = [] {
         BinaryReplacedRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.binaryReplaced = true;
         expect(rule.evaluate(info).has_value());
      };

      "[AlertContainsPathAndReason]"_test = [] {
         BinaryReplacedRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.binaryReplaced = true;
         info.exePath = "/usr/bin/sshd";
         auto alert = rule.evaluate(info);
         const auto* reason = attrValue(alert->attributes, "reason");
         expect(reason != nullptr);
         expect(!reason->empty());
      };
   };
} // namespace
