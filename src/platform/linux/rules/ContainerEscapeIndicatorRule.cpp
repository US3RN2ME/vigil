#include "ContainerEscapeIndicatorRule.hpp"

namespace {
   constexpr uint64_t cap(int bit) {
      return 1ULL << bit;
   }

   constexpr uint64_t kEscapeRelevantCaps = cap(16) | cap(19) | cap(21);
} // namespace

namespace vigil::platform::rules {
   ContainerEscapeIndicatorRule::ContainerEscapeIndicatorRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view ContainerEscapeIndicatorRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> ContainerEscapeIndicatorRule::check(const ProcessInfo& info) {
      if (info.containerId.empty())
         return {};

      if ((info.privilegeMask & kEscapeRelevantCaps) != 0 || info.hasModuleLoad || info.hasPtraceAttach)
         return makeAlert(info);

      return {};
   }

   Alert ContainerEscapeIndicatorRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"cmdline", info.cmdline},
          {"container", info.containerId},
          {"capability_mask", std::to_string(info.privilegeMask)},
      };
      return alert;
   }
} // namespace vigil::platform::rules
