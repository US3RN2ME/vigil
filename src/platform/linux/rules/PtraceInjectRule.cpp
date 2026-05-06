
#include "PtraceInjectRule.hpp"

namespace vigil::platform::linux::rules {
   PtraceInjectRule::PtraceInjectRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view PtraceInjectRule::name() const noexcept {
      return kName;
   }

   Alert PtraceInjectRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"target_pid", std::to_string(info.ptraceTargetPid)},
      };
      return alert;
   }

} // namespace vigil::platform::linux::rules
