#include <filesystem>
#include <fstream>
#include <tuple>

#include "ut_main.hpp"

#include <vigil/Config.hpp>
#include <vigil/Error.hpp>

namespace {
   std::filesystem::path tempConfigPath() {
      return std::filesystem::temp_directory_path() / "vigil_config_test.json";
   }

   suite<"[Config]"> _ = [] {
      using vigil::Config;
      using vigil::ConfigError;

      "[LoadsRuleFields]"_test = [] {
         const auto path = tempConfigPath();
         {
            std::ofstream file{path};
            file << R"({
               "rules": {
                  "test_rule": {
                     "enabled": true,
                     "severity": "critical",
                     "serverNames": ["nginx", "httpd"],
                     "shellNames": ["bash", "sh"],
                     "suspiciousPaths": ["/tmp/", "/dev/shm/"],
                     "cmdlinePatterns": ["encodedcommand", "bash -i"],
                     "suspiciousPorts": [4444, 9001],
                     "protectedProcessNames": ["lsass.exe", "winlogon.exe"]
                  }
               }
            })";
         }

         const auto config = Config::loadFromFile(path.string());
         const auto rule = config.rule("test_rule");

         expect(rule.enabled);
         expect(eq(rule.severity, std::string{"critical"}));
         expect(eq(rule.serverNames.count("nginx"), std::size_t{1}));
         expect(eq(rule.shellNames.count("bash"), std::size_t{1}));
         expect(eq(rule.suspiciousPaths[0], std::string{"/tmp/"}));
         expect(eq(rule.cmdlinePatterns[1], std::string{"bash -i"}));
         expect(eq(rule.suspiciousPorts.count(4444), std::size_t{1}));
         expect(eq(rule.protectedProcessNames.count("lsass.exe"), std::size_t{1}));

         std::filesystem::remove(path);
      };

      "[MissingRuleIsDisabled]"_test = [] {
         const auto path = tempConfigPath();
         {
            std::ofstream file{path};
            file << R"({"rules": {}})";
         }

         const auto config = Config::loadFromFile(path.string());
         const auto rule = config.rule("missing_rule");

         expect(!rule.enabled);
         std::filesystem::remove(path);
      };

      "[ThrowsForMissingFile]"_test = [] {
         bool thrown = false;
         try {
            std::ignore = Config::loadFromFile("definitely_missing_vigil_config.json");
         } catch (const ConfigError&) {
            thrown = true;
         }

         expect(thrown);
      };
   };
} // namespace
