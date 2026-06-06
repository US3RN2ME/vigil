#include "ProcessToken.hpp"

#include "WinApi.hpp"

namespace vigil::platform {
namespace {
TOKEN_INFORMATION_CLASS toNative(ProcessToken::InfoClass infoClass) {
  switch (infoClass) {
  case ProcessToken::InfoClass::User:
    return TokenUser;

  case ProcessToken::InfoClass::Groups:
    return TokenGroups;

  case ProcessToken::InfoClass::Privileges:
    return TokenPrivileges;

  case ProcessToken::InfoClass::IntegrityLevel:
    return TokenIntegrityLevel;

  case ProcessToken::InfoClass::Elevation:
    return TokenElevation;

  case ProcessToken::InfoClass::ElevationType:
    return TokenElevationType;
  }
  return TokenUser;
}
} // namespace

std::optional<ProcessToken> ProcessToken::open(Handle::NativeType process) {
  HANDLE raw = nullptr;

  if (!OpenProcessToken(static_cast<HANDLE>(process), TOKEN_QUERY, &raw))
    return {};

  return Handle{raw};
}

ProcessToken::ProcessToken(Handle token) noexcept : handle_{std::move(token)} {}

std::vector<std::byte> ProcessToken::query(InfoClass infoClass) const {
  if (!handle_.isValid())
    return {};

  DWORD needed = 0;
  const auto nativeClass = toNative(infoClass);

  GetTokenInformation(static_cast<HANDLE>(handle_.native()), nativeClass,
                      nullptr, 0, &needed);

  if (!needed)
    return {};

  std::vector<std::byte> buffer(needed);

  if (!GetTokenInformation(static_cast<HANDLE>(handle_.native()), nativeClass,
                           buffer.data(), needed, &needed)) {
    return {};
  }

  return buffer;
}

} // namespace vigil::platform
