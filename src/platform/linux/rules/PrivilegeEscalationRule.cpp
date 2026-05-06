
#include "PrivilegeEscalationRule.hpp"

namespace vigil::platform::linux::rules {
   PrivilegeEscalationRule::PrivilegeEscalationRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view PrivilegeEscalationRule::name() const noexcept {
      return kName;
   }

   Alert PrivilegeEscalationRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"uid", std::to_string(info.uid)},
          {"euid", std::to_string(info.euid)},
      };
      return alert;
   }
} // namespace vigil::platform::linux::rules
