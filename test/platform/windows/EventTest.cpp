#include "Event.hpp"

#include "ut_main.hpp"

using namespace boost::ut;
using vigil::platform::Event;
using vigil::platform::WaitResult;

suite<"[Event]"> eventTest = [] {
   "[MapsTimeoutAndSignalResults]"_test = [] {
      auto event = Event::create(true, false);
      expect(event.has_value());
      expect(event->wait(0) == WaitResult::Timeout);
      expect(event->set());
      expect(event->wait(0) == WaitResult::Signaled);
   };
};
