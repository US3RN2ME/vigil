
#ifndef VIGIL_RULES_SUSPICIOUSPATHRULE_HPP
#define VIGIL_RULES_SUSPICIOUSPATHRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   class SuspiciousPathRule : public Rule {
  public:
      static constexpr std::string_view kName = "suspicious_path";

      explicit SuspiciousPathRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

  protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_SUSPICIOUSPATHRULE_HPP
