
#ifndef VIGIL_RULES_RULE_HPP
#define VIGIL_RULES_RULE_HPP

#include <optional>

#include <vigil/Alert.hpp>
#include <vigil/ProcessInfo.hpp>
#include <vigil/rules/RuleConfig.hpp>

namespace vigil::rules {
   class Rule {
  public:
      explicit Rule(RuleConfig cfg);

      virtual ~Rule() = default;

      [[nodiscard]] std::optional<Alert> evaluate(const ProcessInfo& info);

      [[nodiscard]] virtual std::string_view name() const noexcept = 0;

  protected:
      [[nodiscard]] virtual std::optional<Alert> check(const ProcessInfo& info) = 0;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const;

      const RuleConfig cfg_;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_RULE_HPP
