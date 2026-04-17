
#ifndef VIGIL_PLATFORM_LINUX_BINARYREPLACEDRULE_HPP
#define VIGIL_PLATFORM_LINUX_BINARYREPLACEDRULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::platform::linux::rules {
   class BinaryReplacedRule : public vigil::rules::Rule {
   public:
      static constexpr std::string_view kName = "binary_replaced_rule";

      explicit BinaryReplacedRule(vigil::rules::RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;
   };
} // namespace vigil::platform::linux::rules

#endif // VIGIL_PLATFORM_LINUX_BINARYREPLACEDRULE_HPP
