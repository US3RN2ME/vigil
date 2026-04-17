
#include <vigil/rules/AnonRwxRule.hpp>

namespace vigil::rules {
   AnonRwxRule::AnonRwxRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view AnonRwxRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> AnonRwxRule::check(const ProcessInfo& info) {
      if (info.hasAnonRwx) {
         return makeAlert(info);
      }
      return {};
   }
} // namespace vigil::rules
