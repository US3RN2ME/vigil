#include <vigil/rules/FilelessExecutionRule.hpp>

namespace vigil::rules {
   FilelessExecutionRule::FilelessExecutionRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view FilelessExecutionRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> FilelessExecutionRule::check(const ProcessInfo& info) {
      if (info.isMemfd || info.exeDeleted || info.imageMissingFromDisk)
         return makeAlert(info);

      return {};
   }

   Alert FilelessExecutionRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"memfd", info.isMemfd ? "true" : "false"},
          {"deleted", info.exeDeleted ? "true" : "false"},
          {"missing_from_disk", info.imageMissingFromDisk ? "true" : "false"},
      };
      return alert;
   }
} // namespace vigil::rules
