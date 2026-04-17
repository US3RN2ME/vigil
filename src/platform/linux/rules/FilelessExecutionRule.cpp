
#include "FilelessExecutionRule.hpp"

namespace vigil::platform::linux::rules {
   FilelessExecutionRule::FilelessExecutionRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view FilelessExecutionRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> FilelessExecutionRule::check(const ProcessInfo& info) {
      if (info.exeDeleted || info.isMemfd) {
         return makeAlert(info);
      }
      return {};
   }
   Alert FilelessExecutionRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      if (info.isMemfd)
         alert.attributes = {{"reason", "memfd — binary never touched disk"}};
      else if (info.exeDeleted)
         alert.attributes = {{"reason", "binary unlinked after exec"}, {"path", info.exePath}};
      return alert;
   }
} // namespace vigil::platform::linux::rules
