
#include "NetworkConnectRule.hpp"

namespace vigil::platform::linux::rules {
   NetworkConnectRule::NetworkConnectRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view NetworkConnectRule::name() const noexcept {
      return kName;
   }

   Alert NetworkConnectRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"dest_addr", info.connectDaddr},
          {"dest_port", std::to_string(info.connectDport)},
      };
      return alert;
   }

} // namespace vigil::platform::linux::rules
