
#ifndef VIGIL_RULEENGINE_HPP
#define VIGIL_RULEENGINE_HPP

#include <memory>
#include <mutex>
#include <unordered_map>

#include <vigil/Alert.hpp>
#include <vigil/AlertDeduplicator.hpp>
#include <vigil/Config.hpp>
#include <vigil/ProcessInfo.hpp>
#include <vigil/Signal.hpp>
#include <vigil/rules/Rule.hpp>

namespace vigil {
   /**
    * @brief Evaluates ProcessInfo snapshots against configured detection rules.
    */
   class RuleEngine {
   public:
      /**
       * @brief Emitted for every alert produced by an enabled rule.
       */
      Signal<Alert> onAlert;

      /**
       * @brief Construct a rule engine with parsed configuration.
       *
       * @param cfg Parsed rule
       * configuration.
       */
      explicit RuleEngine(Config cfg);

      /**
       * @brief Evaluate a process snapshot against all registered rules.
       *
       * @param info Process
       * snapshot to evaluate.
       */
      void process(const ProcessInfo& info);

      /**
       * @brief Register a rule instance. Null pointers are ignored.
       *
       * @param rule Rule instance to
       * register.
       */
      void addRule(std::unique_ptr<rules::Rule> rule);

   private:
      Config cfg_;
      std::mutex rulesMutex_;
      std::vector<std::unique_ptr<rules::Rule>> rules_;
      AlertDeduplicator deduplicator_;
   };

   /**
    * @brief Create a platform-specific rule engine for the current build target.
    *
    * @param cfg Parsed rule
    * configuration.
    *
    * @return Rule engine populated with common and platform-specific rules.
    */
   std::unique_ptr<RuleEngine> createRuleEngine(Config cfg);
} // namespace vigil

#endif // VIGIL_RULEENGINE_HPP
