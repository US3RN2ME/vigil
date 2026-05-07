#include "main.hpp"
#include "rules/BinaryReplacedRule.hpp"

namespace {
   suite<"[BinaryReplacedRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::linux::rules::BinaryReplacedRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenBinaryReplaced]"_test = [] {
         BinaryReplacedRule rule{RuleConfig{}};
         ProcessInfo info;
         info.binaryReplaced = true;
         expect(rule.evaluate(info).has_value());
      };

      "[AlertContainsPathAndReason]"_test = [] {
         BinaryReplacedRule rule{RuleConfig{}};
         ProcessInfo info;
         info.binaryReplaced = true;
         info.exePath        = "/usr/bin/sshd";
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"/usr/bin/sshd"}));
         expect(!alert->attributes[1].second.empty());
      };
   };
} // namespace