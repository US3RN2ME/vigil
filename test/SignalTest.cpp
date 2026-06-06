#include <atomic>
#include <thread>
#include <vector>

#include "ut_main.hpp"

#include <vigil/Signal.hpp>

namespace {
suite<"[Signal]"> _ = [] {
  "[EmitCallsConnectedSlots]"_test = [] {
    vigil::Signal<int> signal;
    int sum = 0;

    signal.connect([&](int value) { sum += value; });
    signal.connect([&](int value) { sum += value * 2; });
    signal.emit(3);

    expect(eq(sum, 9));
  };

  "[DisconnectRemovesSlot]"_test = [] {
    vigil::Signal<int> signal;
    int sum = 0;

    const auto id = signal.connect([&](int value) { sum += value; });
    expect(signal.disconnect(id));
    expect(!signal.disconnect(id));
    signal.emit(3);

    expect(eq(sum, 0));
  };

  "[SlotCanDisconnectItself]"_test = [] {
    vigil::Signal<int> signal;
    int calls = 0;
    uint32_t id = 0;

    id = signal.connect([&](int) {
      ++calls;
      signal.disconnect(id);
    });

    signal.emit(1);
    signal.emit(1);

    expect(eq(calls, 1));
  };

  "[ConcurrentEmissionsAreSafe]"_test = [] {
    vigil::Signal<int> signal;
    std::atomic<int> calls = 0;
    signal.connect([&](int) { ++calls; });

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) {
      threads.emplace_back([&] {
        for (int j = 0; j < 100; ++j)
          signal.emit(1);
      });
    }
    for (auto &thread : threads)
      thread.join();

    expect(eq(calls.load(), 800));
  };

  "[ConcurrentConnectionsAreSafe]"_test = [] {
    vigil::Signal<int> signal;
    std::atomic<int> calls = 0;
    std::vector<std::vector<uint32_t>> ids(8);
    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i) {
      threads.emplace_back([&, i] {
        for (int j = 0; j < 100; ++j) {
          ids[i].emplace_back(signal.connect([&](int) { ++calls; }));
        }
      });
    }
    for (auto &thread : threads)
      thread.join();

    signal.emit(1);
    expect(eq(calls.load(), 800));

    threads.clear();
    for (int i = 0; i < 8; ++i) {
      threads.emplace_back([&, i] {
        for (const auto id : ids[i])
          signal.disconnect(id);
      });
    }
    for (auto &thread : threads)
      thread.join();

    signal.emit(1);
    expect(eq(calls.load(), 800));
  };
};
} // namespace
