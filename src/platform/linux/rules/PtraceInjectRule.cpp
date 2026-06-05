
#include "PtraceInjectRule.hpp"

namespace vigil::platform::rules {
PtraceInjectRule::PtraceInjectRule(vigil::rules::RuleConfig cfg)
    : Rule{std::move(cfg)} {}

std::string_view PtraceInjectRule::name() const noexcept { return kName; }

std::optional<Alert> PtraceInjectRule::check(const ProcessInfo &info) {
  if (info.platform.hasPtraceAttach) {
    return makeAlert(info);
  }
  return {};
}

Alert PtraceInjectRule::makeAlert(const ProcessInfo &info) const {
  auto alert = Rule::makeAlert(info);
  alert.attributes = {
      {"target_pid", std::to_string(info.platform.ptraceTargetPid)}};
  return alert;
}

} // namespace vigil::platform::rules
