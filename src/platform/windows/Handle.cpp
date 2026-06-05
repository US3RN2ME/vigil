
#include "Handle.hpp"

#include <utility>

#include "WinApi.hpp"

namespace vigil::platform {
Handle::Handle(NativeType handle) noexcept : handle_{handle} {}

Handle::~Handle() {
  if (isValid()) {
    CloseHandle(handle_);
  }
}

Handle::Handle(Handle &&other) noexcept
    : handle_{std::exchange(other.handle_, nullptr)} {}

Handle &Handle::operator=(Handle &&other) noexcept {
  if (this != &other) {
    handle_ = std::exchange(other.handle_, nullptr);
  }
  return *this;
}

Handle::NativeType Handle::native() const noexcept { return handle_; }

bool Handle::isValid() const noexcept {
  return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
}

Handle::operator bool() const noexcept { return isValid(); }

void Handle::reset(NativeType handle) noexcept {
  if (isValid()) {
    CloseHandle(handle_);
  }
  handle_ = handle;
}

Handle::NativeType Handle::release() noexcept {
  return std::exchange(handle_, nullptr);
}
} // namespace vigil::platform
