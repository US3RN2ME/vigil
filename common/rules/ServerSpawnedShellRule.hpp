
#ifndef VIGIL_COMMON_RULES_SERVERSPAWNEDSHELLRULE_HPP
#define VIGIL_COMMON_RULES_SERVERSPAWNEDSHELLRULE_HPP

#include "Rule.hpp"
#include "RuleConfig.hpp"

namespace vigil::common::rules {
    class ServerSpawnedShellRule : public Rule {
    public:
        static constexpr std::string_view kName = "server_spawned_shell";

        explicit ServerSpawnedShellRule(RuleConfig cfg);

        [[nodiscard]] std::string_view name() const noexcept override;

    protected:
        [[nodiscard]] std::optional<Alert> check(const ProcessInfo &info) override;
    };
} // vigil

#endif //VIGIL_COMMON_RULES_SERVERSPAWNEDSHELLRULE_HPP
