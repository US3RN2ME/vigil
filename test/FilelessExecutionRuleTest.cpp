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
#if defined(__linux__)
         expect(attrValue(alert->attributes, "deleted") != nullptr);
#else
         expect(attrValue(alert->attributes, "deleted") == nullptr);
#endif
         const auto* missing = attrValue(alert->attributes, "missing_from_disk");
         expect(missing != nullptr);
         expect(eq(*missing, std::string{"true"}));
      };
   };
} // namespace
