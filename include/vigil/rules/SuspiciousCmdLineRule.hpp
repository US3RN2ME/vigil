#ifndef VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP
#define VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP

#include <vigil/rules/Rule.hpp>

namespace vigil::rules {
   class SuspiciousCmdLineRule : public Rule {
   public:
      static constexpr std::string_view kName = "suspicious_cmdline";

      explicit SuspiciousCmdLineRule(RuleConfig cfg);

      [[nodiscard]] std::string_view name() const noexcept override;

   protected:
      [[nodiscard]] std::optional<Alert> check(const ProcessInfo& info) override;

      [[nodiscard]] Alert makeAlert(const ProcessInfo& info) const override;

   private:
      std::vector<std::string> lowerPatterns_;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_SUSPICIOUSCMDLINERULE_HPP
