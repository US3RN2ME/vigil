
#include <format>

#include <vigil/rules/PrivilegeEscalationRuleTest.hpp>

namespace vigil::rules {
   PrivilegeEscalationRule::PrivilegeEscalationRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view PrivilegeEscalationRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> PrivilegeEscalationRule::check(const ProcessInfo& info) {
      auto [it, inserted] = baseline_.emplace(info.pid, info.privilegeMask);
      if (inserted || info.privilegeMask <= it->second) {
         return {};
      }
      it->second = info.privilegeMask;
      return makeAlert(info);
   }

   Alert PrivilegeEscalationRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"mask_before", std::format("{:#x}", baseline_.at(info.pid))},
          {"mask_after", std::format("{:#x}", info.privilegeMask)},
      };
      return alert;
   }
} // namespace vigil::rules
