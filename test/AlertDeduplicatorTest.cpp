#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "ut_main.hpp"

#include <vigil/AlertDeduplicator.hpp>

using namespace boost::ut;
using namespace std::chrono_literals;
using vigil::Alert;
using vigil::AlertDeduplicator;
using vigil::ProcessInfo;

suite<"AlertDeduplicator"> alertDeduplicatorTest = [] {
  "SuppressesIdenticalAlertsDuringCooldown"_test = [] {
    AlertDeduplicator deduplicator{100ms};
    const ProcessInfo info{.pid = 42, .startTimeNs = 7};
    const Alert alert{.rule = "rule", .severity = "high", .info = info};
    constexpr auto start = AlertDeduplicator::Clock::time_point{};

    expect(deduplicator.shouldEmit(alert, start));
    expect(!deduplicator.shouldEmit(alert, start + 99ms));
    expect(deduplicator.shouldEmit(alert, start + 100ms));
  };

  "EmitsChangedEvidence"_test = [] {
    AlertDeduplicator deduplicator{1h};
    const ProcessInfo info{.pid = 42, .startTimeNs = 7};
    const Alert first{
        .rule = "rule",
        .severity = "high",
        .info = info,
        .attributes = {{"matched_pattern", "first"}},
    };
    const Alert second{
        .rule = "rule",
        .severity = "high",
        .info = info,
        .attributes = {{"matched_pattern", "second"}},
    };

    expect(deduplicator.shouldEmit(first));
    expect(deduplicator.shouldEmit(second));
  };

  "EmitsReusedPidForNewProcess"_test = [] {
    AlertDeduplicator deduplicator{1h};
    const ProcessInfo firstInfo{.pid = 42, .startTimeNs = 7};
    const ProcessInfo secondInfo{.pid = 42, .startTimeNs = 8};
    const Alert first{.rule = "rule", .severity = "high", .info = firstInfo};
    const Alert second{.rule = "rule", .severity = "high", .info = secondInfo};

    expect(deduplicator.shouldEmit(first));
    expect(deduplicator.shouldEmit(second));
  };

  "EvictsOldestEntryAtCapacity"_test = [] {
    AlertDeduplicator deduplicator{1h, 2};
    const ProcessInfo firstInfo{.pid = 1};
    const ProcessInfo secondInfo{.pid = 2};
    const ProcessInfo thirdInfo{.pid = 3};
    const Alert first{.rule = "rule", .severity = "high", .info = firstInfo};
    const Alert second{.rule = "rule", .severity = "high", .info = secondInfo};
    const Alert third{.rule = "rule", .severity = "high", .info = thirdInfo};

    expect(deduplicator.shouldEmit(first));
    expect(deduplicator.shouldEmit(second));
    expect(deduplicator.shouldEmit(third));
    expect(deduplicator.shouldEmit(first));
  };

  "AllowsOneConcurrentEmission"_test = [] {
    AlertDeduplicator deduplicator{1h};
    const ProcessInfo info{.pid = 42, .startTimeNs = 7};
    const Alert alert{.rule = "rule", .severity = "high", .info = info};
    std::atomic emitted{0};
    std::vector<std::thread> threads;

    for (auto index = 0; index < 8; ++index) {
      threads.emplace_back([&] {
        if (deduplicator.shouldEmit(alert))
          ++emitted;
      });
    }

    for (auto &thread : threads)
      thread.join();

    expect(eq(emitted.load(), 1));
  };
};
