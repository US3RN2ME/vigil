
#ifndef VIGIL_COMMON_RULES_SUSPICIOUSPATHRULE_HPP
#define VIGIL_COMMON_RULES_SUSPICIOUSPATHRULE_HPP

#include "Rule.hpp"

namespace vigil::common::rules {
    class SuspiciousPathRule : public Rule {
    public:
        static constexpr std::string_view kName = "suspicious_path";

        explicit SuspiciousPathRule(RuleConfig cfg);

        [[nodiscard]] std::string_view name() const noexcept override;

    protected:
        [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;
    };
} // vigil

#endif //VIGIL_COMMON_RULES_SUSPICIOUSPATHRULE_HPP
