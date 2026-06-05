#include <algorithm>

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace vigil::rules::detail {
bool isElevatedProcess(const ProcessInfo &info);
void appendElevationAttributes(Alert &alert, const ProcessInfo &info);
} // namespace vigil::rules::detail

namespace vigil::rules {
ElevatedSuspiciousPathRule::ElevatedSuspiciousPathRule(RuleConfig cfg)
    : Rule{std::move(cfg)} {}

std::string_view ElevatedSuspiciousPathRule::name() const noexcept {
  return kName;
}

std::optional<Alert>
ElevatedSuspiciousPathRule::check(const ProcessInfo &info) {
  if (!detail::isElevatedProcess(info))
    return {};

  const bool fromSuspiciousPath =
      std::ranges::any_of(cfg_.suspiciousPaths, [&info](const auto &prefix) {
        return info.exePath.starts_with(prefix);
      });

  if (fromSuspiciousPath)
    return makeAlert(info);

  return {};
}

Alert ElevatedSuspiciousPathRule::makeAlert(const ProcessInfo &info) const {
  auto alert = Rule::makeAlert(info);
  for (const auto &prefix : cfg_.suspiciousPaths) {
    if (info.exePath.starts_with(prefix)) {
      alert.attributes.emplace_back("matched_path_prefix", prefix);
      break;
    }
  }
  detail::appendElevationAttributes(alert, info);
  return alert;
}
} // namespace vigil::rules
