#include <nlohmann/json.hpp>

#include "ut_main.hpp"

#include <vigil/Alert.hpp>

namespace {
   suite<"[Alert]"> _ = [] {
      "[SerializesProcessSnapshotAndAttributes]"_test = [] {
         vigil::ProcessInfo info{
             .pid = 42,
             .ppid = 7,
             .name = "powershell.exe",
             .exePath = R"(C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe)",
             .cmdline = R"(powershell.exe -EncodedCommand "abc")",
             .privilegeMask = 9,
             .rssBytes = 1024,
             .vszBytes = 4096,
             .threadCount = 3,
             .imageMissingFromDisk = true,
             .hasAnonRwx = true,
             .startTimeNs = 123456,
             .parentName = "cmd.exe",
             .children = {43, 44},
             .hasConnect = true,
             .connectDport = 4444,
             .connectDaddr = "203.0.113.1",
         };

         const vigil::Alert alert{
             .rule = "suspicious_test",
             .severity = "high",
             .info = info,
             .attributes = {{"indicator", "quoted \"value\""}, {"empty", ""}},
         };

         const auto json = nlohmann::json::parse(alert.toJson());
         expect(eq(json.at("rule").get<std::string>(), std::string{"suspicious_test"}));
         expect(eq(json.at("severity").get<std::string>(), std::string{"high"}));
         expect(eq(json.at("process").at("pid"), 42));
         expect(eq(json.at("process").at("children").at(1), 44));
         expect(eq(json.at("process").at("network").at("destination_port"), 4444));
         expect(!json.at("process").at("command_line_truncated").get<bool>());
         expect(eq(json.at("process").at("command_line_original_length"), info.cmdline.size()));
         expect(eq(json.at("attributes").at("indicator").get<std::string>(), std::string{"quoted \"value\""}));
         expect(eq(json.at("attributes").at("empty").get<std::string>(), std::string{}));
      };

      "[TruncatesLongCommandLine]"_test = [] {
         vigil::ProcessInfo info;
         info.cmdline = std::string(1024, 'a');

         const vigil::Alert alert{.rule = "test", .severity = "high", .info = info};
         const auto process = nlohmann::json::parse(alert.toJson()).at("process");

         expect(eq(process.at("command_line").get<std::string>().size(), std::size_t{256}));
         expect(process.at("command_line_truncated").get<bool>());
         expect(eq(process.at("command_line_original_length"), info.cmdline.size()));
      };
   };
} // namespace
