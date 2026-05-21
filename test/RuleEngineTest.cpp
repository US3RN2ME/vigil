#include <stdexcept>

#include "ut_main.hpp"

#include <vigil/RuleEngine.hpp>

namespace {
   class AlwaysAlertRule final : public vigil::rules::Rule {
   public:
      AlwaysAlertRule()
          : Rule{vigil::rules::RuleConfig{.severity = "high"}} {}

      [[nodiscard]] std::string_view name() const noexcept override {
         return "always_alert";
      }

   protected:
      [[nodiscard]] std::optional<vigil::Alert> check(const vigil::ProcessInfo& info) override {
         return makeAlert(info);
      }
   };

   class ThrowingRule final : public vigil::rules::Rule {
   public:
      ThrowingRule()
          : Rule{vigil::rules::RuleConfig{}} {}

      [[nodiscard]] std::string_view name() const noexcept override {
         return "throwing";
      }

   protected:
      [[nodiscard]] std::optional<vigil::Alert> check(const vigil::ProcessInfo&) override {
         throw std::runtime_error{"boom"};
      }
   };

   suite<"[RuleEngine]"> _ = [] {
      "[ProcessEmitsAlertsFromAddedRules]"_test = [] {
         vigil::RuleEngine engine{vigil::Config{}};
         int alerts = 0;
         std::string ruleName;

         engine.onAlert.connect([&](const vigil::Alert& alert) {
            ++alerts;
            ruleName = alert.rule;
         });

         engine.addRule(std::make_unique<AlwaysAlertRule>());
         engine.process(vigil::ProcessInfo{});

         expect(eq(alerts, 1));
         expect(eq(ruleName, std::string{"always_alert"}));
      };

      "[ProcessContinuesAfterRuleException]"_test = [] {
         vigil::RuleEngine engine{vigil::Config{}};
         int alerts = 0;

         engine.onAlert.connect([&](const vigil::Alert&) {
            ++alerts;
         });
         engine.addRule(std::make_unique<ThrowingRule>());
         engine.addRule(std::make_unique<AlwaysAlertRule>());
         engine.process(vigil::ProcessInfo{});

         expect(eq(alerts, 1));
      };

      "[AddNullRuleIsIgnored]"_test = [] {
         vigil::RuleEngine engine{vigil::Config{}};
         int alerts = 0;

         engine.onAlert.connect([&](const vigil::Alert&) {
            ++alerts;
         });
         engine.addRule(nullptr);
         engine.process(vigil::ProcessInfo{});

         expect(eq(alerts, 0));
      };
   };
} // namespace
