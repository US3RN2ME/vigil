#include "StringUtils.hpp"

#include "WinApi.hpp"

namespace vigil::platform {
std::string StringUtils::wideToUtf8(const wchar_t *wide, int size) {
  if (!wide || size <= 0)
    return {};

  const auto required =
      WideCharToMultiByte(CP_UTF8, 0, wide, size, nullptr, 0, nullptr, nullptr);

  if (required <= 0)
    return {};

  std::string utf8(required, '\0');

  const auto written = WideCharToMultiByte(CP_UTF8, 0, wide, size, utf8.data(),
                                           required, nullptr, nullptr);

  if (written <= 0)
    return {};

  return utf8;
}

std::string StringUtils::filenameFromPath(std::string_view path) {
  if (const auto sep = path.find_last_of("\\/"); sep != std::string_view::npos)
    return std::string{path.substr(sep + 1)};
  return std::string{path};
}
} // namespace vigil::platform
