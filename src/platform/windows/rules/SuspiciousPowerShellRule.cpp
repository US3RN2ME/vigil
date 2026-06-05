#include "SuspiciousPowerShellRule.hpp"

#include <algorithm>
#include <cctype>

namespace {
std::string lower(std::string value) {
  std::ranges::transform(value, value.begin(), [](unsigned char ch) {
    return static_cast<char>(std::tolower(ch));
  });
  return value;
}

bool contains(std::string_view value, std::string_view pattern) {
  return value.find(pattern) != std::string_view::npos;
}

std::string matchedIndicator(std::string_view cmdline) {
  for (std::string_view indicator :
       {"-encodedcommand", "frombase64string", "downloadstring",
        "invoke-expression", " iex ", "-nop", "-w hidden",
        "-windowstyle hidden"}) {
    if (contains(cmdline, indicator))
      return std::string{indicator};
  }
  return {};
}
} // namespace

namespace vigil::platform::rules {
SuspiciousPowerShellRule::SuspiciousPowerShellRule(vigil::rules::RuleConfig cfg)
    : Rule{std::move(cfg)} {}

std::string_view SuspiciousPowerShellRule::name() const noexcept {
  return kName;
}

std::optional<Alert> SuspiciousPowerShellRule::check(const ProcessInfo &info) {
  const auto name = lower(info.name);
  if (name != "powershell.exe" && name != "pwsh.exe")
    return {};

  const auto cmdline = lower(info.cmdline);
  const bool suspicious =
      contains(cmdline, "-encodedcommand") ||
      contains(cmdline, "frombase64string") ||
      contains(cmdline, "downloadstring") ||
      contains(cmdline, "invoke-expression") || contains(cmdline, " iex ") ||
      contains(cmdline, "-nop") || contains(cmdline, "-w hidden") ||
      contains(cmdline, "-windowstyle hidden");

  if (suspicious)
    return makeAlert(info);

  return {};
}

Alert SuspiciousPowerShellRule::makeAlert(const ProcessInfo &info) const {
  auto alert = Rule::makeAlert(info);
  alert.attributes = {{"indicator", matchedIndicator(lower(info.cmdline))}};
  return alert;
}
} // namespace vigil::platform::rules
