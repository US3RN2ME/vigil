
#include "LdPreloadHijackRule.hpp"

namespace vigil::platform::rules {
   LdPreloadHijackRule::LdPreloadHijackRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view LdPreloadHijackRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> LdPreloadHijackRule::check(const ProcessInfo& info) {
      if (info.platform.hasLdPreload) {
         return makeAlert(info);
      }
      return {};
   }

   Alert LdPreloadHijackRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {{"env", "LD_PRELOAD"}};
      return alert;
   }
} // namespace vigil::platform::rules
