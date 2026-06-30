
#include "Event.hpp"

#include "WinApi.hpp"

namespace vigil::platform {
   std::optional<Event> Event::create(bool manualReset, bool initialState) noexcept {
      Handle handle{CreateEventW(nullptr, manualReset, initialState, nullptr)};
      if (!handle) {
         return {};
      }
      return Event{std::move(handle)};
   }

   bool Event::set() noexcept {
      return SetEvent(static_cast<HANDLE>(handle_.native()));
   }

   bool Event::reset() noexcept {
      return ResetEvent(static_cast<HANDLE>(handle_.native()));
   }

   WaitResult Event::wait(uint32_t milliseconds) const noexcept {
      switch (WaitForSingleObject(static_cast<HANDLE>(handle_.native()), milliseconds)) {
         case WAIT_OBJECT_0:
            return WaitResult::Signaled;
         case WAIT_TIMEOUT:
            return WaitResult::Timeout;
         default:
            return WaitResult::Failed;
      }
   }

   Event::Event(Handle handle) noexcept
       : handle_{std::move(handle)} {}
} // namespace vigil::platform
