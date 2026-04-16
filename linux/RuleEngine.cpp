
#include "RuleEngine.hpp"

namespace vigil::common {
    std::unique_ptr<RuleEngine> createRuleEngine(Config cfg) {
        return std::make_unique<RuleEngine>(std::move(cfg));
    }
}

namespace vigil::linux {
    RuleEngine::RuleEngine(common::Config cfg)
        : common::RuleEngine{std::move(cfg)} {
        //TODO: add more rules here
    }
}
