#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <utility>

#include <vigil/Alert.hpp>

namespace vigil::detail {
namespace {
constexpr std::size_t kMaxSerializedCommandLineLength = 256;

std::string_view truncatedCommandLine(std::string_view commandLine) {
  return commandLine.substr(0, kMaxSerializedCommandLineLength);
}
} // namespace

std::string serializeAlert(const Alert &alert, nlohmann::json platformJson) {
  nlohmann::json attributesJson = nlohmann::json::object();
  for (const auto &[name, value] : alert.attributes) {
    attributesJson[std::string{name}] = value;
  }

  return nlohmann::json{
      {"rule", std::string{alert.rule}},
      {"severity", std::string{alert.severity}},
      {"process",
       {
           {"pid", alert.info.pid},
           {"ppid", alert.info.ppid},
           {"name", alert.info.name},
           {"parent_name", alert.info.parentName},
           {"executable_path", alert.info.exePath},
           {"command_line", truncatedCommandLine(alert.info.cmdline)},
           {"command_line_truncated",
            alert.info.cmdline.size() > kMaxSerializedCommandLineLength},
           {"command_line_original_length", alert.info.cmdline.size()},
           {"privilege_mask", alert.info.privilegeMask},
           {"rss_bytes", alert.info.rssBytes},
           {"vsz_bytes", alert.info.vszBytes},
           {"thread_count", alert.info.threadCount},
           {"image_missing_from_disk", alert.info.imageMissingFromDisk},
           {"has_anonymous_rwx", alert.info.hasAnonRwx},
           {"start_time_ns", alert.info.startTimeNs},
           {"children", alert.info.children},
           {"network",
            {
                {"has_connect", alert.info.hasConnect},
                {"destination_port", alert.info.connectDport},
                {"destination_address", alert.info.connectDaddr},
            }},
           {"platform", std::move(platformJson)},
       }},
      {"attributes", std::move(attributesJson)},
  }
      .dump();
}
} // namespace vigil::detail
