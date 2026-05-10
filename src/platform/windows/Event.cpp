
#include "Event.hpp"

#include "WinApi.hpp"

namespace vigil::platform::windows {
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
      return static_cast<WaitResult>(WaitForSingleObject(static_cast<HANDLE>(handle_.native()), milliseconds));
   }

   Event::Event(Handle handle) noexcept
       : handle_{std::move(handle)} {}
} // namespace vigil::platform::windows
