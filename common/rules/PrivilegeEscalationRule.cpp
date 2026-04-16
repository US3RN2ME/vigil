
#include "PrivilegeEscalationRule.hpp"

namespace vigil::common::rules {
    PrivilegeEscalationRule::PrivilegeEscalationRule(RuleConfig cfg)
        : Rule{std::move(cfg)} {
    }

    std::string_view PrivilegeEscalationRule::name() const noexcept {
        return kName;
    }

    std::optional<Alert> PrivilegeEscalationRule::check(const ProcessInfo &info) {
        const auto [it, inserted] = baseline_.emplace(info.pid, info.privilegeMask);
        if (inserted || info.privilegeMask <= it->second) {
            return {};
        }
        it->second = info.privilegeMask;
        return makeAlert(info);
    }
} // vigil
