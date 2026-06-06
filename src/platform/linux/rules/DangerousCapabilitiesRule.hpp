#ifndef VIGIL_PLATFORM_LINUX_RULES_DANGEROUSCAPABILITIESRULE_HPP
#define VIGIL_PLATFORM_LINUX_RULES_DANGEROUSCAPABILITIESRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::rules {
class DangerousCapabilitiesRule : public vigil::rules::Rule {
public:
  static constexpr std::string_view kName = "dangerous_capabilities";

  explicit DangerousCapabilitiesRule(vigil::rules::RuleConfig cfg);

  [[nodiscard]] std::string_view name() const noexcept override;

protected:
  [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;

  [[nodiscard]] Alert makeAlert(const ProcessInfo &info) const override;

private:
  uint64_t matchedMask_ = 0;
};
} // namespace vigil::platform::rules

#endif // VIGIL_PLATFORM_LINUX_RULES_DANGEROUSCAPABILITIESRULE_HPP
