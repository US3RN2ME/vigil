#include <algorithm>
#include <cctype>

#include <vigil/rules/SuspiciousCmdLineRule.hpp>

namespace {
   std::string toLower(std::string value) {
      std::ranges::transform(value, value.begin(), [](unsigned char ch) {
         return static_cast<char>(std::tolower(ch));
      });
      return value;
   }

   bool contains(std::string_view value, std::string_view pattern) {
      return value.find(pattern) != std::string_view::npos;
   }
} // namespace

namespace vigil::rules {
   SuspiciousCmdLineRule::SuspiciousCmdLineRule(RuleConfig cfg)
       : Rule{std::move(cfg)} {
      lowerPatterns_.reserve(cfg_.cmdlinePatterns.size());
      for (const auto& pattern : cfg_.cmdlinePatterns)
         lowerPatterns_.push_back(toLower(pattern));
   }

   std::string_view SuspiciousCmdLineRule::name() const noexcept {
      return kName;
   }

   std::optional<Alert> SuspiciousCmdLineRule::check(const ProcessInfo& info) {
      if (info.cmdline.empty() || lowerPatterns_.empty())
         return {};

      const auto lowerCmdline = toLower(info.cmdline);
      for (const auto& pattern : lowerPatterns_) {
         if (contains(lowerCmdline, pattern))
            return makeAlert(info);
      }

      return {};
   }

   Alert SuspiciousCmdLineRule::makeAlert(const ProcessInfo& info) const {
      const auto lowerCmdline = toLower(info.cmdline);

      std::string matched;
      for (std::size_t i = 0; i < lowerPatterns_.size(); ++i) {
         if (contains(lowerCmdline, lowerPatterns_[i])) {
            matched = cfg_.cmdlinePatterns[i];
            break;
         }
      }

      auto alert = Rule::makeAlert(info);
      alert.attributes = {{"matched_pattern", matched}};
      return alert;
   }
} // namespace vigil::rules
