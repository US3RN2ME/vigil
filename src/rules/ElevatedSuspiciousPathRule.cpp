#include <algorithm>

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace {
   bool isElevated(const vigil::ProcessInfo& info) {
#if defined(__linux__)
      return info.platform.euid == 0;
#elif defined(_WIN32)
      using Integrity = vigil::ProcessInfo::PlatformInfo::Integrity;
      return info.platform.integrity == Integrity::High || info.platform.integrity == Integrity::System;
#else
      (void)info;
      return false;
#endif
   }

   std::string uidToString(const vigil::ProcessInfo& info) {
#if defined(__linux__)
      return std::to_string(info.platform.uid);
#else
      (void)info;
      return "n/a";
#endif
   }

   std::string euidToString(const vigil::ProcessInfo& info) {
#if defined(__linux__)
      return std::to_string(info.platform.euid);
#else
      (void)info;
      return "n/a";
#endif
   }

   std::string integrityToString(const vigil::ProcessInfo& info) {
#if defined(_WIN32)
      using Integrity = vigil::ProcessInfo::PlatformInfo::Integrity;
      switch (info.platform.integrity) {
         case Integrity::Low:
            return "low";
         case Integrity::Medium:
            return "medium";
         case Integrity::High:
            return "high";
         case Integrity::System:
            return "system";
         case Integrity::Unknown:
            return "unknown";
      }
#else
      (void)info;
#endif
      return "n/a";
   }

} // namespace

namespace vigil::rules {
   ElevatedSuspiciousPathRule::ElevatedSuspiciousPathRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view ElevatedSuspiciousPathRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> ElevatedSuspiciousPathRule::check(const ProcessInfo& info) {
      if (!isElevated(info))
         return {};

      const bool fromSuspiciousPath = std::ranges::any_of(cfg_.suspiciousPaths, [&info](const auto& prefix) {
         return info.exePath.starts_with(prefix);
      });

      if (fromSuspiciousPath)
         return makeAlert(info);

      return {};
   }

   Alert ElevatedSuspiciousPathRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},     {"cmdline", info.cmdline},    {"parent", info.parentName},
          {"uid", uidToString(info)}, {"euid", euidToString(info)}, {"integrity", integrityToString(info)},
      };
      return alert;
   }
} // namespace vigil::rules
