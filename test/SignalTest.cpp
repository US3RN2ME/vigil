#include "ut_main.hpp"

#include <vigil/Signal.hpp>

namespace {
   suite<"[Signal]"> _ = [] {
      "[EmitCallsConnectedSlots]"_test = [] {
         vigil::Signal<int> signal;
         int sum = 0;

         signal.connect([&](int value) {
            sum += value;
         });
         signal.connect([&](int value) {
            sum += value * 2;
         });
         signal.emit(3);

         expect(eq(sum, 9));
      };

      "[DisconnectRemovesSlot]"_test = [] {
         vigil::Signal<int> signal;
         int sum = 0;

         const auto id = signal.connect([&](int value) {
            sum += value;
         });
         expect(signal.disconnect(id));
         expect(!signal.disconnect(id));
         signal.emit(3);

         expect(eq(sum, 0));
      };
   };
} // namespace
