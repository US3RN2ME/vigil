#include <vigil/rules/FilelessExecutionRule.hpp>

namespace vigil::rules::detail {
bool isMemfdProcess(const ProcessInfo &info);
bool isDeletedExecutable(const ProcessInfo &info);
} // namespace vigil::rules::detail

namespace vigil::rules {
FilelessExecutionRule::FilelessExecutionRule(RuleConfig cfg)
    : Rule{std::move(cfg)} {}

std::string_view FilelessExecutionRule::name() const noexcept { return kName; }

std::optional<Alert> FilelessExecutionRule::check(const ProcessInfo &info) {
  if (detail::isMemfdProcess(info) || detail::isDeletedExecutable(info) ||
      info.imageMissingFromDisk)
    return makeAlert(info);

  return {};
}

Alert FilelessExecutionRule::makeAlert(const ProcessInfo &info) const {
  auto alert = Rule::makeAlert(info);
  if (detail::isMemfdProcess(info))
    alert.attributes.emplace_back("memfd", "true");
  if (detail::isDeletedExecutable(info))
    alert.attributes.emplace_back("deleted", "true");
  if (info.imageMissingFromDisk)
    alert.attributes.emplace_back("missing_from_disk", "true");
  return alert;
}
} // namespace vigil::rules
