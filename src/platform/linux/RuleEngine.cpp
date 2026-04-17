
#include <vigil/RuleEngine.hpp>

namespace vigil {
   std::unique_ptr<RuleEngine> createRuleEngine(Config cfg) {
      auto engine = std::make_unique<RuleEngine>(std::move(cfg));
      // engine->addRule(std::make_unique<rules::LinuxSpecificRule>(...));
      return engine;
   }
} // namespace vigil
