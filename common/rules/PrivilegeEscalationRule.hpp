
#ifndef VIGIL_COMMON_RULES_PRIVILEGEESCALATIONRULE_HPP
#define VIGIL_COMMON_RULES_PRIVILEGEESCALATIONRULE_HPP

#include "Rule.hpp"

namespace vigil::common::rules {
    class PrivilegeEscalationRule : public Rule {
    public:
        static constexpr std::string_view kName = "privilege_escalation";

        explicit PrivilegeEscalationRule(RuleConfig cfg);

        [[nodiscard]] std::string_view name() const noexcept override;

    protected:
        [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;

    private:
        std::unordered_map<uint32_t, uint32_t> baseline_;
    };
} // vigil

#endif //VIGIL_COMMON_RULES_PRIVILEGEESCALATIONRULE_HPP
