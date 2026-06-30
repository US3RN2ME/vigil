#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "ut_main.hpp"

#include <vigil/AlertSink.hpp>

namespace {
   std::filesystem::path tempAlertPath() {
      return std::filesystem::temp_directory_path() / "vigil_alert_sink_test.ndjson";
   }

   std::filesystem::path tempConfigPath() {
      return std::filesystem::temp_directory_path() / "vigil_alert_sink_config_test.json";
   }

   suite<"[AlertSink]"> _ = [] {
      "[JsonFileSinkWritesNewlineDelimitedJson]"_test = [] {
         const auto alertPath = tempAlertPath();
         const auto configPath = tempConfigPath();
         std::filesystem::remove(alertPath);

         {
            std::ofstream file{configPath};
            file << R"({"alertSinks": [{"type": "json_file", "path": ")" << alertPath.generic_string()
                 << R"("}], "rules": {}})";
         }

         {
            auto dispatcher = vigil::createAlertDispatcher(vigil::Config::loadFromFile(configPath.string()));

            vigil::ProcessInfo info{.pid = 42, .name = "cmd.exe", .exePath = R"(C:\Windows\System32\cmd.exe)"};
            const vigil::Alert alert{.rule = "test_rule", .severity = "high", .info = info};
            dispatcher->emit(alert);
         }

         {
            std::ifstream in{alertPath};
            std::string line;
            std::getline(in, line);
            const auto json = nlohmann::json::parse(line);

            expect(eq(json.at("rule").get<std::string>(), std::string{"test_rule"}));
            expect(eq(json.at("severity").get<std::string>(), std::string{"high"}));
            expect(eq(json.at("process").at("pid"), 42));
            expect(in.peek() == std::char_traits<char>::eof());
         }

         std::filesystem::remove(alertPath);
         std::filesystem::remove(configPath);
      };
   };
} // namespace
