#include "ProcessSnapshot.hpp"

#include "WinApi.hpp"

namespace vigil::platform {
ProcessSnapshot::ProcessSnapshot(Handle handle) noexcept
    : handle_{std::move(handle)} {}

std::optional<ProcessSnapshot> ProcessSnapshot::create() {
  Handle handle{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)};
  if (!handle)
    return {};
  return ProcessSnapshot{std::move(handle)};
}

void ProcessSnapshot::forEach(
    const std::function<void(uint32_t pid)> &callback) const {
  if (!handle_.isValid())
    return;

  PROCESSENTRY32W entry{};
  entry.dwSize = sizeof(entry);

  const auto h = static_cast<HANDLE>(handle_.native());

  if (!Process32FirstW(h, &entry))
    return;

  do {
    if (entry.th32ProcessID != 0)
      callback(static_cast<uint32_t>(entry.th32ProcessID));
  } while (Process32NextW(h, &entry));
}
} // namespace vigil::platform
