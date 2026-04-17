
#ifndef VIGIL_ALERT_HPP
#define VIGIL_ALERT_HPP

#include <string_view>

#include <vigil/ProcessInfo.hpp>

namespace vigil {
   struct Alert {
      std::string_view rule;
      std::string_view severity;
      const ProcessInfo& info;
      std::vector<std::pair<std::string_view, std::string>> attributes;
   };
} // namespace vigil

#endif // VIGIL_ALERT_HPP
