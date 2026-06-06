
#include <vigil/rules/AnonRwxRuleTest.hpp>

namespace vigil::rules {
AnonRwxRule::AnonRwxRule(RuleConfig cfg) : Rule{std::move(cfg)} {}

std::string_view AnonRwxRule::name() const noexcept { return kName; }

std::optional<Alert> AnonRwxRule::check(const ProcessInfo &info) {
  if (info.hasAnonRwx) {
    return makeAlert(info);
  }
  return {};
}

Alert AnonRwxRule::makeAlert(const ProcessInfo &info) const {
  auto alert = Rule::makeAlert(info);
  alert.attributes = {{"memory", "anonymous_rwx"}};
  return alert;
}
} // namespace vigil::rules
