
#ifndef VIGIL_COMMON_RULES_ANONRWXRULE_HPP
#define VIGIL_COMMON_RULES_ANONRWXRULE_HPP

#include "Rule.hpp"

namespace vigil::common::rules {
    class AnonRwxRule : public Rule {
    public:
        static constexpr std::string_view kName = "anon_rwx_mapping";

        explicit AnonRwxRule(RuleConfig cfg);

        [[nodiscard]] std::string_view name() const noexcept override;

    protected:
        [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;
    };
} // vigil

#endif //VIGIL_COMMON_RULES_ANONRWXRULE_HPP
