#include "DebugPrivilegeRule.hpp"

namespace vigil::platform::rules {

   DebugPrivilegeRule::DebugPrivilegeRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view DebugPrivilegeRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> DebugPrivilegeRule::check(const ProcessInfo& info) {
      if (info.privilegeMask & kSeDebugPrivilege)
         return makeAlert(info);
      return {};
   }

   Alert DebugPrivilegeRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {{"privilege", "SeDebugPrivilege"}};
      return alert;
   }

} // namespace vigil::platform::rules
