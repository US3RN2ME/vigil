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
      if (isMemfd(info))
         alert.attributes.emplace_back("memfd", "true");
      if (isDeletedExecutable(info))
         alert.attributes.emplace_back("deleted", "true");
      if (info.imageMissingFromDisk)
         alert.attributes.emplace_back("missing_from_disk", "true");
      return alert;
   }
} // namespace vigil::rules
