#include <nlohmann/json.hpp>

#include "ut_main.hpp"

#include <vigil/Alert.hpp>

namespace {
   suite<"[AlertPlatform]"> _ = [] {
      "[SerializesWindowsProcessMetadata]"_test = [] {
         vigil::ProcessInfo info;
         info.platform.integrity = vigil::ProcessInfo::PlatformInfo::Integrity::High;
         info.platform.jobObjectName = "test-job";

         const vigil::Alert alert{.rule = "test", .severity = "high", .info = info};
         const auto platform = nlohmann::json::parse(alert.toJson()).at("process").at("platform");

         expect(eq(platform.at("integrity").get<std::string>(), std::string{"high"}));
         expect(eq(platform.at("job_object_name").get<std::string>(), std::string{"test-job"}));
      };
   };
} // namespace
