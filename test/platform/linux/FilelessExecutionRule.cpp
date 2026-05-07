#include "main.hpp"
#include "rules/FilelessExecutionRule.hpp"

namespace {
   suite<"[FilelessExecutionRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::linux::rules::FilelessExecutionRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenExeDeleted]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.exeDeleted = true;
         expect(rule.evaluate(info).has_value());
      };

      "[FiresWhenMemfd]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.isMemfd = true;
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
         info.isMemfd = true;
         expect(!rule.evaluate(info).has_value());
         info.exeDeleted = true;
         auto alert = rule.evaluate(info);
         expect(alert->attributes[0].second.contains("memfd"));
      };

      "[MemfdAlertAttributeDescribesDisk]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.isMemfd = true;
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].first, std::string_view{"reason"}));
         expect(alert->attributes[0].second.contains("memfd"));

      };

      "[DeletedAlertAttributesContainPath]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.exeDeleted = true;
         info.exePath    = "/tmp/payload (deleted)";
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[1].second, std::string{"/tmp/payload (deleted)"}));
      };
   };
} // namespace