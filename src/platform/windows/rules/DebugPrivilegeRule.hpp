
#ifndef VIGIL_PLATFORM_RULES_DEBUGPRIVILEGERULE_HPP
#define VIGIL_PLATFORM_RULES_DEBUGPRIVILEGERULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::rules {

   class DebugPrivilegeRule : public vigil::rules::Rule {
   public:
      static constexpr std::string_view kName = "debug_privilege_rule";

      explicit DebugPrivilegeRule(vigil::rules::RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;
      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;

   private:
      // SE_DEBUG_PRIVILEGE has LUID 20; ProcessInfoReader encodes it as bit 20.
      static constexpr uint64_t kSeDebugPrivilege = 1ULL << 20;
   };

} // namespace vigil::platform::rules

#endif // VIGIL_PLATFORM_RULES_DEBUGPRIVILEGERULE_HPP
