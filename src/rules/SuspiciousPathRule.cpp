
#include <algorithm>

#include <vigil/rules/SuspiciousPathRuleTest.hpp>

namespace vigil::rules {
   SuspiciousPathRule::SuspiciousPathRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view SuspiciousPathRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> SuspiciousPathRule::check(const ProcessInfo& info) {
      auto matches = std::ranges::any_of(cfg_.suspiciousPaths, [&info](const auto& prefix) {
         return info.exePath.starts_with(prefix);
      });

      if (matches) {
         return makeAlert(info);
      }
      return {};
   }

   Alert SuspiciousPathRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      for (const auto& prefix : cfg_.suspiciousPaths) {
         if (info.exePath.starts_with(prefix)) {
            alert.attributes = {{"matched_path_prefix", prefix}};
            break;
         }
      }
      return alert;
   }
} // namespace vigil::rules
