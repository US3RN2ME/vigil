#include "ut_main.hpp"

#include <vigil/rules/FilelessExecutionRule.hpp>

namespace {
   suite<"[FilelessExecutionRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::FilelessExecutionRule;
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

      "[FiresWhenImageMissingFromDisk]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.imageMissingFromDisk = true;

         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
         info.isMemfd = true;

         expect(!rule.evaluate(info).has_value());
      };

      "[AlertAttributesDescribeImageBacking]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.exeDeleted = true;
         info.imageMissingFromDisk = true;
         info.exePath = "/tmp/payload (deleted)";

         const auto alert = rule.evaluate(info);

         expect(alert.has_value());
         expect(eq(alert->attributes[0].second, info.exePath));
         expect(eq(alert->attributes[2].second, std::string{"true"}));
         expect(eq(alert->attributes[3].second, std::string{"true"}));
      };
   };
} // namespace
