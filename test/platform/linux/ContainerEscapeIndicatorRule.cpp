#include "rules/ContainerEscapeIndicatorRule.hpp"

#include "ut_main.hpp"

namespace {
   suite<"[ContainerEscapeIndicatorRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::rules::ContainerEscapeIndicatorRule;
      using vigil::rules::RuleConfig;

      "[FiresForContainerWithSysPtrace]"_test = [] {
         ContainerEscapeIndicatorRule rule{RuleConfig{}};
         ProcessInfo info;
         info.containerId = "docker/abc";
         info.privilegeMask = 1ULL << 19;

         expect(rule.evaluate(info).has_value());
      };

      "[DoesNotFireOutsideContainer]"_test = [] {
         ContainerEscapeIndicatorRule rule{RuleConfig{}};
         ProcessInfo info;
         info.privilegeMask = 1ULL << 19;

         expect(!rule.evaluate(info).has_value());
      };
   };
} // namespace
