
#ifndef VIGIL_ALERT_HPP
#define VIGIL_ALERT_HPP

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <vigil/ProcessInfo.hpp>

namespace vigil {
   /**
    * @brief Security finding produced by a rule evaluation.
    *
    * Alerts borrow the ProcessInfo supplied to RuleEngine::process(), so
    * consumers should copy any data they need after the callback returns.
    */
   struct Alert {
      /**
       * @brief Stable rule identifier, matching the rule key in the JSON config.
       */
      std::string_view rule;

      /**
       * @brief Configured alert severity, such as "low", "medium", "high", or
       * "critical".
       */
      std::string_view severity;

      /**
       * @brief Process snapshot that caused the rule to fire.
       */
      const ProcessInfo& info;

      /**
       * @brief Rule-specific evidence fields, excluding common process
       * context already available in @ref info.
       */
      std::vector<std::pair<std::string_view, std::string>> attributes{};

      /**
       * @brief Serialize the alert and its process snapshot as JSON.
       */
      [[nodiscard]] std::string toJson() const;
   };
} // namespace vigil

#endif // VIGIL_ALERT_HPP
