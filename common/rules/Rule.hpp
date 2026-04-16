
#ifndef VIGIL_COMMON_RULES_RULE_HPP
#define VIGIL_COMMON_RULES_RULE_HPP

#include <optional>

#include "RuleConfig.hpp"
#include "common/ProcessInfo.hpp"
#include "common/Alert.hpp"

namespace vigil::common::rules {
    class Rule {
    public:
        explicit Rule(RuleConfig cfg);

        virtual ~Rule() = default;

        [[nodiscard]] std::optional<Alert> evaluate(const ProcessInfo &info);

        [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    protected:
        [[nodiscard]] virtual std::optional<Alert> check(const ProcessInfo &info) = 0;

        [[nodiscard]] Alert makeAlert(const ProcessInfo &info) const;

        const RuleConfig cfg_;
    };
} // vigil

#endif //VIGIL_COMMON_RULES_RULE_HPP
