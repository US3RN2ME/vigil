
#ifndef VIGIL_PLATFORM_LINUX_LDPRELOADHIJACKRULE_HPP
#define VIGIL_PLATFORM_LINUX_LDPRELOADHIJACKRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::linux::rules {
   class LdPreloadHijackRule : public vigil::rules::Rule {
   public:
      static constexpr std::string_view kName = "ld_preload_hijack_rule";

      explicit LdPreloadHijackRule(vigil::rules::RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::platform::linux::rules

#endif // VIGIL_PLATFORM_LINUX_LDPRELOADHIJACKRULE_HPP
