
#include <format>

#include <vigil/rules/PrivilegeEscalationRuleTest.hpp>

namespace vigil::rules {
   PrivilegeEscalationRule::PrivilegeEscalationRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view PrivilegeEscalationRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> PrivilegeEscalationRule::check(const ProcessInfo& info) {
      auto [it, inserted] = baseline_.emplace(info.pid, Baseline{info.startTimeNs, info.privilegeMask});
      if (inserted)
         return {};

      auto& baseline = it->second;
      if (baseline.startTimeNs != info.startTimeNs) {
         baseline = Baseline{info.startTimeNs, info.privilegeMask};
         return {};
      }

      const auto previousMask = baseline.privilegeMask;
      baseline.privilegeMask = info.privilegeMask;
      if (info.privilegeMask <= previousMask)
         return {};

      alertBaseline_ = previousMask;
      return makeAlert(info);
   }

   Alert PrivilegeEscalationRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"mask_before", std::format("{:#x}", alertBaseline_)},
          {"mask_after", std::format("{:#x}", info.privilegeMask)},
      };
      return alert;
   }
} // namespace vigil::rules
