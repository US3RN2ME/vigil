
#ifndef VIGIL_LINUX_RULEENGINE_HPP
#define VIGIL_LINUX_RULEENGINE_HPP

#include "common/RuleEngine.hpp"

namespace vigil::linux {
    class RuleEngine : public common::RuleEngine {
    public:
        explicit RuleEngine(common::Config cfg);
    };
}

#endif //VIGIL_LINUX_RULEENGINE_HPP
