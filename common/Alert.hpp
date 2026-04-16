
#ifndef VIGIL_COMMON_ALERT_HPP
#define VIGIL_COMMON_ALERT_HPP

#include "ProcessInfo.hpp"
#include <string_view>

namespace vigil::common {
    struct Alert {
        std::string_view rule;
        std::string_view severity;
        const ProcessInfo &info;
    };
}

#endif //VIGIL_COMMON_ALERT_HPP
