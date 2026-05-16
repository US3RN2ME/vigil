
#include "PrivilegeEscalationRule.hpp"

namespace vigil::platform::rules {
   PrivilegeEscalationRule::PrivilegeEscalationRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view PrivilegeEscalationRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> PrivilegeEscalationRule::check(const ProcessInfo& info) {
      if (info.platform.hasSetuidToRoot) {
         return makeAlert(info);
      }
      return {};
   }

   Alert PrivilegeEscalationRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"uid", std::to_string(info.platform.uid)},
          {"euid", std::to_string(info.platform.euid)},
      };
      return alert;
   }
} // namespace vigil::platform::rules
