#include <string>

#include <vigil/rules/ElevatedSuspiciousPathRule.hpp>

namespace vigil::rules::detail {
   bool isElevatedProcess(const ProcessInfo& info) {
      return info.platform.euid == 0;
   }

   void appendElevationAttributes(Alert& alert, const ProcessInfo& info) {
      alert.attributes.emplace_back("uid", std::to_string(info.platform.uid));
      alert.attributes.emplace_back("euid", std::to_string(info.platform.euid));
   }
} // namespace vigil::rules::detail
