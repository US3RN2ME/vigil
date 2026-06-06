#include <fstream>
#include <nlohmann/json.hpp>

#include <vigil/Config.hpp>
#include <vigil/Error.hpp>
#include <vigil/Logger.hpp>

namespace {
constexpr std::string_view kRules = "rules";
constexpr std::string_view kAlertSinks = "alertSinks";
constexpr std::string_view kType = "type";
constexpr std::string_view kEnabled = "enabled";
constexpr std::string_view kPath = "path";
constexpr std::string_view kFormat = "format";
constexpr std::string_view kSeverity = "severity";
constexpr std::string_view kServerNames = "serverNames";
constexpr std::string_view kShellNames = "shellNames";
constexpr std::string_view kSuspiciousPaths = "suspiciousPaths";
constexpr std::string_view kCmdlinePatterns = "cmdlinePatterns";
constexpr std::string_view kSuspiciousPorts = "suspiciousPorts";
constexpr std::string_view kProtectedProcessNames = "protectedProcessNames";
constexpr std::string_view kExcludePaths = "excludePaths";
constexpr std::string_view kExcludeParentNames = "excludeParentNames";

vigil::AlertSinkType parseAlertSinkType(const std::string &type) {
  if (type == "stdout")
    return vigil::AlertSinkType::Stdout;
  if (type == "json_file")
    return vigil::AlertSinkType::JsonFile;
  if (type == "syslog")
    return vigil::AlertSinkType::Syslog;

  throw vigil::ConfigError{"unknown alert sink type: '{}'", type};
}

vigil::SyslogAlertFormat parseSyslogFormat(const std::string &format) {
  if (format == "cef")
    return vigil::SyslogAlertFormat::Cef;
  if (format == "leef")
    return vigil::SyslogAlertFormat::Leef;

  throw vigil::ConfigError{"unknown syslog alert format: '{}'", format};
}
} // namespace

namespace vigil {
static const rules::RuleConfig kDisabledRule{.enabled = false};

Config Config::loadFromFile(const std::string_view path) {
  std::ifstream f{path.data()};
  if (!f)
    throw ConfigError{std::format("cannot open config file: '{}'", path)};

  try {
    const auto json = nlohmann::json::parse(f, nullptr, true, true);

    Config cfg;

    for (const auto &[name, ruleJson] :
         json.value(kRules.data(), nlohmann::json::object()).items()) {
      rules::RuleConfig rc;
      rc.enabled = ruleJson.value(kEnabled.data(), true);
      rc.severity = ruleJson.value(kSeverity.data(), "medium");

      if (ruleJson.contains(kServerNames))
        for (const auto &item : ruleJson.at(kServerNames))
          rc.serverNames.insert(item.get<std::string>());

      if (ruleJson.contains(kShellNames))
        for (const auto &item : ruleJson.at(kShellNames))
          rc.shellNames.insert(item.get<std::string>());

      if (ruleJson.contains(kSuspiciousPaths))
        for (const auto &item : ruleJson.at(kSuspiciousPaths))
          rc.suspiciousPaths.push_back(item.get<std::string>());

      if (ruleJson.contains(kCmdlinePatterns))
        for (const auto &item : ruleJson.at(kCmdlinePatterns))
          rc.cmdlinePatterns.push_back(item.get<std::string>());

      if (ruleJson.contains(kSuspiciousPorts))
        for (const auto &item : ruleJson.at(kSuspiciousPorts))
          rc.suspiciousPorts.insert(item.get<uint16_t>());

      if (ruleJson.contains(kProtectedProcessNames))
        for (const auto &item : ruleJson.at(kProtectedProcessNames))
          rc.protectedProcessNames.insert(item.get<std::string>());

      if (ruleJson.contains(kExcludePaths))
        for (const auto &item : ruleJson.at(kExcludePaths))
          rc.excludePaths.push_back(item.get<std::string>());

      if (ruleJson.contains(kExcludeParentNames))
        for (const auto &item : ruleJson.at(kExcludeParentNames))
          rc.excludeParentNames.insert(item.get<std::string>());

      cfg.rules_.emplace(name, std::move(rc));
    }

    if (json.contains(kAlertSinks)) {
      cfg.alertSinks_.clear();

      for (const auto &sinkJson : json.at(kAlertSinks)) {
        AlertSinkConfig sink;
        sink.enabled = sinkJson.value(kEnabled.data(), true);
        sink.type = parseAlertSinkType(sinkJson.value(kType.data(), "stdout"));
        sink.path = sinkJson.value(kPath.data(), "");
        sink.format = parseSyslogFormat(sinkJson.value(kFormat.data(), "cef"));

        if (sink.enabled && sink.type == AlertSinkType::JsonFile &&
            sink.path.empty())
          throw ConfigError{"json_file alert sink requires a non-empty path"};

        cfg.alertSinks_.push_back(std::move(sink));
      }
    }

    log::info("config loaded from {}", path);
    return cfg;
  } catch (const std::exception &e) {
    throw ConfigError{"failed to parse config '{}': {}", path, e.what()};
  }
}

rules::RuleConfig Config::rule(const std::string_view name) const noexcept {
  const auto it = rules_.find(std::string{name});
  return it != rules_.end() ? it->second : kDisabledRule;
}

const std::vector<AlertSinkConfig> &Config::alertSinks() const noexcept {
  return alertSinks_;
}
} // namespace vigil
