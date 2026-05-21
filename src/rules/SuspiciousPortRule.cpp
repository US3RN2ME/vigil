#include <vigil/rules/SuspiciousPortRule.hpp>

namespace vigil::rules {
   SuspiciousPortRule::SuspiciousPortRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view SuspiciousPortRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> SuspiciousPortRule::check(const ProcessInfo& info) {
      if (!info.hasConnect)
         return {};

      if (cfg_.suspiciousPorts.contains(info.connectDport))
         return makeAlert(info);

      return {};
   }

   Alert SuspiciousPortRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"dest_addr", info.connectDaddr},
          {"dest_port", std::to_string(info.connectDport)},
      };
      return alert;
   }
} // namespace vigil::rules
