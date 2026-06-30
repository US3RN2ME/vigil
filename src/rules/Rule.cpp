
#include <vigil/rules/Rule.hpp>

namespace {
   bool isPathSeparator(const char ch) {
      return ch == '/' || ch == '\\';
   }

   bool matchesExcludedPath(const std::string& path, const std::string& exclude) {
      if (exclude.empty())
         return false;

      if (isPathSeparator(exclude.back()))
         return path.starts_with(exclude);

      return path == exclude;
   }

   bool isExcluded(const vigil::rules::RuleConfig& cfg, const vigil::ProcessInfo& info) {
      for (const auto& path : cfg.excludePaths) {
         if (matchesExcludedPath(info.exePath, path))
            return true;
      }

      return !info.parentName.empty() && cfg.excludeParentNames.contains(info.parentName);
   }
} // namespace

namespace vigil::rules {
   Rule::Rule(RuleConfig cfg)
       : cfg_{std::move(cfg)} {}

   std::optional<Alert> Rule::evaluate(const ProcessInfo& info) {
      if (!cfg_.enabled || isExcluded(cfg_, info)) {
         return {};
      }
      return check(info);
   }

   Alert Rule::makeAlert(const ProcessInfo& info) const {
      return Alert{.rule = name(), .severity = cfg_.severity, .info = info};
   }
} // namespace vigil::rules
