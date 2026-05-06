#include "main.hpp"
#include "rules/LdPreloadHijackRule.hpp"

namespace {
   suite<"[LdPreloadHijackRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::linux::rules::LdPreloadHijackRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenLdPreloadSet]"_test = [] {
         LdPreloadHijackRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasLdPreload = true;
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenLdPreloadClear]"_test = [] {
         LdPreloadHijackRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasLdPreload = false;
         expect(!rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [] {
         LdPreloadHijackRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
         info.hasLdPreload = true;
         expect(!rule.evaluate(info).has_value());
      };

      "[AlertContainsPathAndCmdline]"_test = [] {
         LdPreloadHijackRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasLdPreload = true;
         info.exePath      = "/usr/bin/curl";
         info.cmdline      = "curl https://example.com";
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[0].second, std::string{"/usr/bin/curl"}));
         expect(eq(alert->attributes[1].second, std::string{"curl https://example.com"}));
      };
   };
} // namespace