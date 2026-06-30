#include <nlohmann/json.hpp>

#include "ut_main.hpp"

#include <vigil/Alert.hpp>

namespace {
   suite<"[AlertPlatform]"> _ = [] {
      "[SerializesLinuxProcessMetadata]"_test = [] {
         vigil::ProcessInfo info;
         info.platform.uid = 1000;
         info.platform.euid = 0;
         info.platform.containerId = "test-container";

         const vigil::Alert alert{.rule = "test", .severity = "high", .info = info};
         const auto platform = nlohmann::json::parse(alert.toJson()).at("process").at("platform");

         expect(eq(platform.at("uid"), 1000));
         expect(eq(platform.at("euid"), 0));
         expect(eq(platform.at("container_id").get<std::string>(), std::string{"test-container"}));
      };
   };
} // namespace
