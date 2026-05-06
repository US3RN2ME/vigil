
#ifndef VIGIL_PLATFORM_LINUX_RULES_NETWORKCONNECTRULE_HPP
#define VIGIL_PLATFORM_LINUX_RULES_NETWORKCONNECTRULE_HPP

#include "vigil/rules/Rule.hpp"

namespace vigil::platform::linux::rules {
   class NetworkConnectRule : public vigil::rules::Rule {
   public:
      static constexpr std::string_view kName = "network_connect_rule";

      explicit NetworkConnectRule(vigil::rules::RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::platform::linux::rules

#endif // VIGIL_PLATFORM_LINUX_RULES_NETWORKCONNECTRULE_HPP
