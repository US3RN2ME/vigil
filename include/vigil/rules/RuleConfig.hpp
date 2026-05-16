
#ifndef VIGIL_RULES_RULECONFIG_HPP
#define VIGIL_RULES_RULECONFIG_HPP

#include <cstdint>
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
      std::vector<std::string> cmdlinePatterns;
      std::unordered_set<uint16_t> suspiciousPorts;
      std::unordered_set<std::string> protectedProcessNames;
   };
} // namespace vigil::rules

#endif // VIGIL_RULES_RULECONFIG_HPP
