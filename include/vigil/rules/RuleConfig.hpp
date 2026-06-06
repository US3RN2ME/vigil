
#ifndef VIGIL_RULES_RULECONFIG_HPP
#define VIGIL_RULES_RULECONFIG_HPP

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace vigil::rules {
/**
 * @brief Canonical severity levels used by rules and configuration.
 */
enum Severity { Low, Medium, High, Critical };

/**
 * @brief Configurable parameters shared by built-in rules.
 */
struct RuleConfig {
  /**
   * @brief Whether the rule is active.
   */
  bool enabled = true;

  /**
   * @brief Alert severity string emitted when the rule fires.
   */
  std::string severity = "medium";

  /**
   * @brief Parent process names considered server processes.
   */
  std::unordered_set<std::string> serverNames;

  /**
   * @brief Child process names considered interactive shells.
   */
  std::unordered_set<std::string> shellNames;

  /**
   * @brief Path prefixes considered suspicious execution locations.
   */
  std::vector<std::string> suspiciousPaths;

  /**
   * @brief Case-insensitive command-line substrings considered suspicious.
   */
  std::vector<std::string> cmdlinePatterns;

  /**
   * @brief Destination ports considered suspicious for outbound connects.
   */
  std::unordered_set<uint16_t> suspiciousPorts;

  /**
   * @brief Protected system process names that should only run from trusted
   * paths.
   */
  std::unordered_set<std::string> protectedProcessNames;

  /**
   * @brief Executable paths excluded from this rule.
   *
   * Entries ending with a path separator are treated as prefixes. Other entries
   * must match the process executable path exactly.
   */
  std::vector<std::string> excludePaths;

  /**
   * @brief Parent process names excluded from this rule.
   */
  std::unordered_set<std::string> excludeParentNames;
};
} // namespace vigil::rules

#endif // VIGIL_RULES_RULECONFIG_HPP
