
#include "KernelModuleLoadRule.hpp"

namespace vigil::platform::linux::rules {
   KernelModuleLoadRule::KernelModuleLoadRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view KernelModuleLoadRule::name() const noexcept {
      return kName;
   }

   Alert KernelModuleLoadRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"uid", std::to_string(info.uid)},
      };
      return alert;
   }

} // namespace vigil::platform::linux::rules
