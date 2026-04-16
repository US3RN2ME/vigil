
#include "Config.hpp"

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace {
    constexpr std::string_view kRules = "rules";
    constexpr std::string_view kEnabled = "enabled";
    constexpr std::string_view kSeverity = "severity";
    constexpr std::string_view kServerNames = "serverNames";
    constexpr std::string_view kShellNames = "shellNames";
    constexpr std::string_view kSuspiciousPaths = "suspiciousPaths";
}

namespace vigil::common {
    static const rules::RuleConfig kDisabledRule{.enabled = false};

    Config Config::loadFromFile(const std::string_view path) {
        std::ifstream f{path.data()};
        if (!f) throw std::runtime_error{std::string{"cannot open config file: "} + path.data()};

        const auto json = nlohmann::json::parse(
            f,
            nullptr,
            true,
            true);

        Config cfg;

        for (const auto &[name, ruleJson]:
             json.value(kRules.data(), nlohmann::json::object()).items()) {
            rules::RuleConfig rc;
            rc.enabled = ruleJson.value(kEnabled.data(), true);
            rc.severity = ruleJson.value(kSeverity.data(), "medium");

            if (ruleJson.contains(kServerNames))
                for (const auto &item: ruleJson.at(kServerNames))
                    rc.serverNames.insert(item.get<std::string>());

            if (ruleJson.contains(kShellNames))
                for (const auto &item: ruleJson.at(kShellNames))
                    rc.shellNames.insert(item.get<std::string>());

            if (ruleJson.contains(kSuspiciousPaths))
                for (const auto &item: ruleJson.at(kSuspiciousPaths))
                    rc.suspiciousPaths.push_back(item.get<std::string>());

            cfg.rules_.emplace(name, std::move(rc));
        }

        return cfg;
    }

    rules::RuleConfig Config::rule(const std::string_view name) const noexcept {
        const auto it = rules_.find(std::string{name});
        return it != rules_.end() ? it->second : kDisabledRule;
    }
}
