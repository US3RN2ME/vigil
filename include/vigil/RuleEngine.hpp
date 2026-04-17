
#ifndef VIGIL_RULEENGINE_HPP
#define VIGIL_RULEENGINE_HPP

#include <memory>
#include <unordered_map>

#include <vigil/Alert.hpp>
#include <vigil/Config.hpp>
#include <vigil/ProcessInfo.hpp>
#include <vigil/Signal.hpp>
#include <vigil/rules/Rule.hpp>

namespace vigil {
   class RuleEngine {
   public:
      Signal<Alert> onAlert;

      explicit RuleEngine(Config cfg);

      void process(const ProcessInfo& info);
      void addRule(std::unique_ptr<rules::Rule> rule);

   private:
      Config cfg_;
      std::vector<std::unique_ptr<rules::Rule>> rules_;
   };

   std::unique_ptr<RuleEngine> createRuleEngine(Config cfg);
} // namespace vigil

#endif // VIGIL_RULEENGINE_HPP
