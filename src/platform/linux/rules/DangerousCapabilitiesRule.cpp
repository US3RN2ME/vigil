#include "DangerousCapabilitiesRule.hpp"

#include <format>

namespace {
   constexpr uint64_t cap(int bit) {
      return 1ULL << bit;
   }

   constexpr uint64_t kDangerousCaps = cap(2) |  // CAP_DAC_READ_SEARCH
                                       cap(12) | // CAP_NET_ADMIN
                                       cap(16) | // CAP_SYS_MODULE
                                       cap(19) | // CAP_SYS_PTRACE
                                       cap(21);  // CAP_SYS_ADMIN
} // namespace

namespace vigil::platform::rules {
   DangerousCapabilitiesRule::DangerousCapabilitiesRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view DangerousCapabilitiesRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> DangerousCapabilitiesRule::check(const ProcessInfo& info) {
      if (info.euid == 0)
         return {};

      matchedMask_ = info.privilegeMask & kDangerousCaps;
      if (matchedMask_ != 0)
         return makeAlert(info);

      return {};
   }

   Alert DangerousCapabilitiesRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"uid", std::to_string(info.uid)},
          {"euid", std::to_string(info.euid)},
          {"capability_mask", std::format("{:#x}", matchedMask_)},
      };
      return alert;
   }
} // namespace vigil::platform::rules
