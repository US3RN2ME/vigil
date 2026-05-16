#include <vigil/rules/FilelessExecutionRule.hpp>

namespace {
   bool isMemfd(const vigil::ProcessInfo& info) {
#if defined(__linux__)
      return info.platform.isMemfd;
#else
      (void)info;
      return false;
#endif
   }

   bool isDeletedExecutable(const vigil::ProcessInfo& info) {
#if defined(__linux__)
      return info.platform.exeDeleted;
#else
      (void)info;
      return false;
#endif
   }
} // namespace

namespace vigil::rules {
   FilelessExecutionRule::FilelessExecutionRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {}

   std::string_view FilelessExecutionRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> FilelessExecutionRule::check(const ProcessInfo& info) {
      if (isMemfd(info) || isDeletedExecutable(info) || info.imageMissingFromDisk)
         return makeAlert(info);

      return {};
   }

   Alert FilelessExecutionRule::makeAlert(const ProcessInfo& info) const {
      auto alert = Rule::makeAlert(info);
      alert.attributes = {
          {"path", info.exePath},
          {"memfd", isMemfd(info) ? "true" : "false"},
          {"deleted", isDeletedExecutable(info) ? "true" : "false"},
          {"missing_from_disk", info.imageMissingFromDisk ? "true" : "false"},
      };
      return alert;
   }
} // namespace vigil::rules
