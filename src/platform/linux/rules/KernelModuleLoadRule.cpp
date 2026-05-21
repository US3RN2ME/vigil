
#include "KernelModuleLoadRule.hpp"

namespace vigil::platform::rules {
   KernelModuleLoadRule::KernelModuleLoadRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view KernelModuleLoadRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> KernelModuleLoadRule::check(const ProcessInfo& info) {
      if (info.platform.hasModuleLoad) {
         return makeAlert(info);
      }
      return {};
   }

   Alert KernelModuleLoadRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"event", "module_load"},
          {"uid", std::to_string(info.platform.uid)},
      };
      return alert;
   }

} // namespace vigil::platform::rules
