
#ifndef VIGIL_RULES_RULECONFIG_HPP
#define VIGIL_RULES_RULECONFIG_HPP

#include <string>
#include <unordered_set>
#include <vector>

namespace vigil::rules {
   enum Severity { Low, Medium, High, Critical };

   struct RuleConfig {
      bool enabled = true;
      std::string severity = "medium";
      std::unordered_set<std::string> serverNames;
      std::unordered_set<std::string> shellNames;
      std::vector<std::string> suspiciousPaths;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_RULECONFIG_HPP
