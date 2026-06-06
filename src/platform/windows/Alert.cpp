#include <nlohmann/json.hpp>
#include <string_view>

#include <vigil/Alert.hpp>

namespace vigil {
namespace {
std::string_view toStringView(ProcessInfo::PlatformInfo::Integrity integrity) {
  using Integrity = ProcessInfo::PlatformInfo::Integrity;

  switch (integrity) {
  case Integrity::Low:
    return "low";
  case Integrity::Medium:
    return "medium";
  case Integrity::High:
    return "high";
  case Integrity::System:
    return "system";
  case Integrity::Unknown:
    return "unknown";
  }

  return "unknown";
}
} // namespace

namespace detail {
std::string serializeAlert(const Alert &alert, nlohmann::json platformJson);
} // namespace detail

std::string Alert::toJson() const {
  return detail::serializeAlert(
      *this, {
                 {"integrity", toStringView(info.platform.integrity)},
                 {"job_object_name", info.platform.jobObjectName},
             });
}
} // namespace vigil
