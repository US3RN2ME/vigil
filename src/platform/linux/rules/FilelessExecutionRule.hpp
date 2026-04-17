
#ifndef VIGIL_PLATFORM_LINUX_FILELESSEXECUTIONRULE_HPP
#define VIGIL_PLATFORM_LINUX_FILELESSEXECUTIONRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::linux::rules {
   class FilelessExecutionRule : public vigil::rules::Rule {
   public:
      static constexpr std::string_view kName = "fileless_execution_rule";

      explicit FilelessExecutionRule(vigil::rules::RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::platform::linux::rules

#endif // VIGIL_PLATFORM_LINUX_FILELESSEXECUTIONRULE_HPP
