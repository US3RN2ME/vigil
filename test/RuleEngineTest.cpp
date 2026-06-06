#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <vector>

#include "ut_main.hpp"

#include <vigil/RuleEngine.hpp>

namespace {
class AlwaysAlertRule final : public vigil::rules::Rule {
public:
  AlwaysAlertRule() : Rule{vigil::rules::RuleConfig{.severity = "high"}} {}

  [[nodiscard]] std::string_view name() const noexcept override {
    return "always_alert";
  }

protected:
  [[nodiscard]] std::optional<vigil::Alert>
  check(const vigil::ProcessInfo &info) override {
    return makeAlert(info);
  }
};

class ThrowingRule final : public vigil::rules::Rule {
public:
  ThrowingRule() : Rule{vigil::rules::RuleConfig{}} {}

  [[nodiscard]] std::string_view name() const noexcept override {
    return "throwing";
  }

protected:
  [[nodiscard]] std::optional<vigil::Alert>
  check(const vigil::ProcessInfo &) override {
    throw std::runtime_error{"boom"};
  }
};

class ConcurrentEvaluationRule final : public vigil::rules::Rule {
public:
  ConcurrentEvaluationRule() : Rule{vigil::rules::RuleConfig{}} {}

  [[nodiscard]] std::string_view name() const noexcept override {
    return "concurrent_evaluation";
  }

  [[nodiscard]] int maxActive() const noexcept { return maxActive_; }

protected:
  [[nodiscard]] std::optional<vigil::Alert>
  check(const vigil::ProcessInfo &) override {
    const int active = ++active_;
    int observed = maxActive_;
    while ((active > observed) &&
           !maxActive_.compare_exchange_weak(observed, active)) {
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{1});
    --active_;
    return {};
  }

private:
  std::atomic<int> active_{0};
  std::atomic<int> maxActive_{0};
};

suite<"[RuleEngine]"> _ = [] {
  "[ProcessEmitsAlertsFromAddedRules]"_test = [] {
    vigil::RuleEngine engine{vigil::Config{}};
    int alerts = 0;
    std::string ruleName;

    engine.onAlert.connect([&](const vigil::Alert &alert) {
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

    engine.onAlert.connect([&](const vigil::Alert &) { ++alerts; });
    engine.addRule(std::make_unique<ThrowingRule>());
    engine.addRule(std::make_unique<AlwaysAlertRule>());
    engine.process(vigil::ProcessInfo{});

    expect(eq(alerts, 1));
  };

  "[AddNullRuleIsIgnored]"_test = [] {
    vigil::RuleEngine engine{vigil::Config{}};
    int alerts = 0;

    engine.onAlert.connect([&](const vigil::Alert &) { ++alerts; });
    engine.addRule(nullptr);
    engine.process(vigil::ProcessInfo{});

    expect(eq(alerts, 0));
  };

  "[ConcurrentProcessCallsSerializeRuleEvaluation]"_test = [] {
    vigil::RuleEngine engine{vigil::Config{}};
    auto rule = std::make_unique<ConcurrentEvaluationRule>();
    auto *rulePtr = rule.get();
    engine.addRule(std::move(rule));

    const vigil::ProcessInfo info;
    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
      threads.emplace_back([&] { engine.process(info); });
    for (auto &thread : threads)
      thread.join();

    expect(eq(rulePtr->maxActive(), 1));
  };

  "[SuppressesDuplicateAlerts]"_test = [] {
    vigil::RuleEngine engine{vigil::Config{}};
    std::atomic alerts{0};
    engine.onAlert.connect([&](const vigil::Alert &) { ++alerts; });
    engine.addRule(std::make_unique<AlwaysAlertRule>());
    const vigil::ProcessInfo info{.pid = 42, .startTimeNs = 7};

    engine.process(info);
    engine.process(info);

    expect(eq(alerts.load(), 1));
  };
};
} // namespace
