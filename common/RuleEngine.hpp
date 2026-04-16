
#ifndef VIGIL_COMMON_RULEENGINE_HPP
#define VIGIL_COMMON_RULEENGINE_HPP

#include "ProcessInfo.hpp"
#include "Signal.hpp"
#include "Alert.hpp"

#include <memory>
#include <unordered_map>

#include "Config.hpp"
#include "rules/Rule.hpp"

namespace vigil::common {
    class RuleEngine {
    public:
        Signal<Alert> onAlert;

        explicit RuleEngine(Config cfg);

        virtual ~RuleEngine() = default;

        void process(const ProcessInfo &info);

    protected:
        void addRule(std::unique_ptr<rules::Rule> rule);

        const Config cfg_;

    private:
        std::vector<std::unique_ptr<rules::Rule> > rules_;
    };

    [[nodiscard]] std::unique_ptr<RuleEngine> createRuleEngine(Config cfg);
}

#endif //VIGIL_COMMON_RULEENGINE_HPP
