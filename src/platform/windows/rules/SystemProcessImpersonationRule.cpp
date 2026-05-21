#include "SystemProcessImpersonationRule.hpp"

#include <algorithm>
#include <cctype>

namespace {
   std::string lower(std::string value) {
      std::ranges::transform(value, value.begin(), [](unsigned char ch) {
         return static_cast<char>(std::tolower(ch));
      });
      return value;
   }

   bool isProtectedName(const vigil::rules::RuleConfig& cfg, std::string_view name) {
      return std::ranges::any_of(cfg.protectedProcessNames, [name](const auto& protectedName) {
         return lower(protectedName) == name;
      });
   }
} // namespace

namespace vigil::platform::rules {
   SystemProcessImpersonationRule::SystemProcessImpersonationRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view SystemProcessImpersonationRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> SystemProcessImpersonationRule::check(const ProcessInfo& info) {
      const auto processName = lower(info.name);
      if (!isProtectedName(cfg_, processName))
         return {};

      const auto path = lower(info.exePath);
      const bool inSystemRoot = path.starts_with("c:\\windows\\system32\\") || path.starts_with("c:\\windows\\syswow64\\");

      if (!inSystemRoot)
         return makeAlert(info);

      return {};
   }

   Alert SystemProcessImpersonationRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {{"expected_path", R"(C:\Windows\System32\ or C:\Windows\SysWOW64\)"}};
      return alert;
   }
} // namespace vigil::platform::rules
