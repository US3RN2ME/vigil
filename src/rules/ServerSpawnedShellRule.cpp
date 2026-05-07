
#include <vigil/rules/ServerSpawnedShellRuleTest.hpp>

namespace {
   [[nodiscard]] std::string stripExe(const std::string& name) noexcept {
      if (name.ends_with(".exe"))
         return name.substr(0, name.size() - 4);
      return name;
   }
} // namespace

namespace vigil::rules {
   ServerSpawnedShellRule::ServerSpawnedShellRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view ServerSpawnedShellRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> ServerSpawnedShellRule::check(const ProcessInfo& info) {
      auto parentIsServer = cfg_.serverNames.contains(stripExe(info.parentName));
      auto childIsShell = cfg_.shellNames.contains(stripExe(info.name));
      if (parentIsServer && childIsShell) {
         return makeAlert(info);
      }
      return {};
   }

   Alert ServerSpawnedShellRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"parent", info.parentName},
          {"child", info.name},
      };
      return alert;
   }
} // namespace vigil::rules
