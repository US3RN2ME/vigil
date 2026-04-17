
#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   Rule::Rule(RuleConfig cfg) : cfg_{std::move(cfg)} {}

   std::optional<Alert> Rule::evaluate(const ProcessInfo& info) {
      if (!cfg_.enabled) {
         return {};
      }
      return check(info);
   }

   Alert Rule::makeAlert(const ProcessInfo& info) const {
      return Alert{.rule = name(), .severity = cfg_.severity, .info = info};
   }
} // namespace vigil::rules
