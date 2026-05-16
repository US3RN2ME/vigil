#include "ut_main.hpp"

#include <vigil/rules/FilelessExecutionRule.hpp>

namespace {
   suite<"[FilelessExecutionRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::rules::FilelessExecutionRule;
      using vigil::rules::RuleConfig;

#if defined(__linux__)
      "[FiresWhenExeDeleted]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.exeDeleted = true;

         expect(rule.evaluate(info).has_value());
      };

      "[FiresWhenMemfd]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.platform.isMemfd = true;

         expect(rule.evaluate(info).has_value());
      };
#endif

      "[FiresWhenImageMissingFromDisk]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
         info.imageMissingFromDisk = true;

         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
#if defined(__linux__)
         info.platform.isMemfd = true;
#else
         info.imageMissingFromDisk = true;
#endif

         expect(!rule.evaluate(info).has_value());
      };

      "[AlertAttributesDescribeImageBacking]"_test = [] {
         FilelessExecutionRule rule{RuleConfig{}};
         ProcessInfo info;
#if defined(__linux__)
         info.platform.exeDeleted = true;
#endif
         info.imageMissingFromDisk = true;
         info.exePath = "/tmp/payload (deleted)";

         const auto alert = rule.evaluate(info);

         expect(alert.has_value());
         expect(eq(alert->attributes[0].second, info.exePath));
#if defined(__linux__)
         expect(eq(alert->attributes[2].second, std::string{"true"}));
#else
         expect(eq(alert->attributes[2].second, std::string{"false"}));
#endif
         expect(eq(alert->attributes[3].second, std::string{"true"}));
      };
   };
} // namespace
