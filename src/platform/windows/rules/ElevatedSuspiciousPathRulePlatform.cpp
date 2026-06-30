#include <string>

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace {
   std::string integrityToString(vigil::ProcessInfo::PlatformInfo::Integrity integrity) {
      using Integrity = vigil::ProcessInfo::PlatformInfo::Integrity;

      switch (integrity) {
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

      return "unknown";
   }
} // namespace

namespace vigil::rules::detail {
   bool isElevatedProcess(const ProcessInfo& info) {
      using Integrity = ProcessInfo::PlatformInfo::Integrity;
      return info.platform.integrity == Integrity::High || info.platform.integrity == Integrity::System;
   }

   void appendElevationAttributes(Alert& alert, const ProcessInfo& info) {
      alert.attributes.emplace_back("integrity", integrityToString(info.platform.integrity));
   }
} // namespace vigil::rules::detail
