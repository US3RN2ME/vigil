#include "LolbinExecutionRule.hpp"

#include <algorithm>
#include <array>
#include <cctype>

namespace {
   std::string lower(std::string value) {
      std::ranges::transform(value, value.begin(), [](unsigned char ch) {
         return static_cast<char>(std::tolower(ch));
      });
      return value;
   }

   bool containsAny(std::string_view value, const std::initializer_list<std::string_view> patterns) {
      return std::ranges::any_of(patterns, [value](std::string_view pattern) {
         return value.find(pattern) != std::string_view::npos;
      });
   }
} // namespace

namespace vigil::platform::rules {
   LolbinExecutionRule::LolbinExecutionRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view LolbinExecutionRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> LolbinExecutionRule::check(const ProcessInfo& info) {
      const auto name = lower(info.name);
      const auto cmdline = lower(info.cmdline);

      if (name == "rundll32.exe" && containsAny(cmdline, {"javascript:", "url.dll,fileprotocolhandler", "http://", "https://"}))
         return makeAlert(info);

      if (name == "regsvr32.exe" && containsAny(cmdline, {"/i:http", "/i:https", "scrobj.dll"}))
         return makeAlert(info);

      if (name == "mshta.exe" && containsAny(cmdline, {"http://", "https://", "vbscript:", "javascript:"}))
         return makeAlert(info);

      if (name == "certutil.exe" && containsAny(cmdline, {" -urlcache ", " -decode ", " -decodehex "}))
         return makeAlert(info);

      if (name == "bitsadmin.exe" && containsAny(cmdline, {" /transfer ", " /create ", "http://", "https://"}))
         return makeAlert(info);

      if ((name == "wscript.exe" || name == "cscript.exe") &&
          containsAny(cmdline, {"http://", "https://", "\\appdata\\local\\temp\\", "\\windows\\temp\\"}))
         return makeAlert(info);

      return {};
   }

   Alert LolbinExecutionRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"name", info.name},
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"parent", info.parentName},
      };
      return alert;
   }
} // namespace vigil::platform::rules
