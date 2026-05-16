#include <algorithm>

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace {
   bool isElevated(const vigil::ProcessInfo& info) {
      if (info.euid == 0)
         return true;

      return info.integrity == vigil::ProcessInfo::Integrity::High || info.integrity == vigil::ProcessInfo::Integrity::System;
   }

   std::string integrityToString(vigil::ProcessInfo::Integrity integrity) {
      switch (integrity) {
         case vigil::ProcessInfo::Integrity::Low:
            return "low";
         case vigil::ProcessInfo::Integrity::Medium:
            return "medium";
         case vigil::ProcessInfo::Integrity::High:
            return "high";
         case vigil::ProcessInfo::Integrity::System:
            return "system";
         case vigil::ProcessInfo::Integrity::Unknown:
            return "unknown";
      }
      return "unknown";
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
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"parent", info.parentName},
          {"uid", std::to_string(info.uid)},
          {"euid", std::to_string(info.euid)},
          {"integrity", integrityToString(info.integrity)},
      };
      return alert;
   }
} // namespace vigil::rules
