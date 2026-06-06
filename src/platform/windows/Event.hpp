
#ifndef VIGIL_PLATFORM_EVENT_HPP
#define VIGIL_PLATFORM_EVENT_HPP

#include <optional>

#include "Handle.hpp"

namespace vigil::platform {
enum class WaitResult {
  Signaled,
  Timeout,
  Failed,
};

class Event {
public:
  explicit Event() = default;

  explicit Event(Handle handle) noexcept;

  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;

  Event(Event &&) noexcept = default;
  Event &operator=(Event &&) noexcept = default;

  [[nodiscard]] static std::optional<Event> create(bool manualReset,
                                                   bool initialState) noexcept;

  bool set() noexcept;
  bool reset() noexcept;
  WaitResult wait(uint32_t milliseconds) const noexcept;

private:
  Handle handle_;
};
} // namespace vigil::platform

#endif // VIGIL_PLATFORM_EVENT_HPP
