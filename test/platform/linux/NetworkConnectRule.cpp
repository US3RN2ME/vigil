#include "main.hpp"
#include "rules/NetworkConnectRule.hpp"

namespace {
   suite<"[NetworkConnectRule]"> _ = [] {
      using vigil::ProcessInfo;
      using vigil::platform::linux::rules::NetworkConnectRule;
      using vigil::rules::RuleConfig;

      "[FiresWhenConnectObserved]"_test = [] {
         NetworkConnectRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasConnect = true;
         expect(rule.evaluate(info).has_value());
      };

      "[SilentWhenNoConnect]"_test = [] {
         NetworkConnectRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasConnect = false;
         expect(!rule.evaluate(info).has_value());
      };

      "[SilentWhenDisabled]"_test = [] {
         NetworkConnectRule rule{RuleConfig{.enabled = false}};
         ProcessInfo info;
         info.hasConnect = true;
         expect(!rule.evaluate(info).has_value());
      };

      "[AlertContainsDestAddrAndPort]"_test = [] {
         NetworkConnectRule rule{RuleConfig{}};
         ProcessInfo info;
         info.hasConnect   = true;
         info.exePath      = "/usr/bin/curl";
         info.connectDaddr = "1.2.3.4";
         info.connectDport = 443;
         auto alert = rule.evaluate(info);
         expect(eq(alert->attributes[1].second, std::string{"1.2.3.4"}));
         expect(eq(alert->attributes[2].second, std::string{"443"}));
      };
   };
} // namespace