
#include "BinaryReplacedRule.hpp"

namespace vigil::platform::rules {
   BinaryReplacedRule::BinaryReplacedRule(vigil::rules::RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view BinaryReplacedRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> BinaryReplacedRule::check(const ProcessInfo& info) {
      if (info.platform.binaryReplaced) {
         return makeAlert(info);
      }
      return {};
   }
   Alert BinaryReplacedRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"reason", "inode mismatch — file replaced on disk after exec"},
      };
      return alert;
   }
} // namespace vigil::platform::rules
