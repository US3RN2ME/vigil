#include <format>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

#include <vigil/AlertSink.hpp>
#include <vigil/Error.hpp>
#include <vigil/Logger.hpp>

namespace {
   class StdoutSink final : public vigil::AlertSink {
   public:
      void emit(const vigil::Alert& alert) override {
         vigil::log::warn("[ALERT] {}", alert.toJson());
      }
   };

   class JsonFileSink final : public vigil::AlertSink {
   public:
      explicit JsonFileSink(const std::string& path)
          : out_{path, std::ios::app} {
         if (!out_)
            throw vigil::ConfigError{"cannot open alert sink file: '{}'", path};
      }

      void emit(const vigil::Alert& alert) override {
         const std::lock_guard lock{mutex_};
         out_ << alert.toJson() << '\n';
         out_.flush();
      }

   private:
      std::mutex mutex_;
      std::ofstream out_;
   };

   std::string escapeCef(std::string value) {
      std::string escaped;
      escaped.reserve(value.size());

      for (const char ch : value) {
         if (ch == '\\' || ch == '=' || ch == '|')
            escaped.push_back('\\');
         if (ch == '\n' || ch == '\r')
            escaped.push_back(' ');
         else
            escaped.push_back(ch);
      }

      return escaped;
   }

   std::string escapeLeef(std::string value) {
      std::string escaped;
      escaped.reserve(value.size());

      for (const char ch : value) {
         if (ch == '\\' || ch == '\t' || ch == '|')
            escaped.push_back('\\');
         if (ch == '\n' || ch == '\r')
            escaped.push_back(' ');
         else
            escaped.push_back(ch);
      }

      return escaped;
   }

   std::string jsonString(const nlohmann::json& json, std::string_view pointer) {
      const auto& value = json.at(nlohmann::json::json_pointer{std::string{pointer}});
      return value.is_string() ? value.get<std::string>() : value.dump();
   }

   std::string formatCef(const vigil::Alert& alert) {
      const auto json = nlohmann::json::parse(alert.toJson());
      const auto pid = json.at("process").at("pid").dump();
      const auto ppid = json.at("process").at("ppid").dump();
      const auto name = jsonString(json, "/process/name");
      const auto path = jsonString(json, "/process/executable_path");
      const auto cmdline = jsonString(json, "/process/command_line");

      return std::format("CEF:0|vigil|vigil|1|{}|{}|{}|dproc={} sproc={} cs1Label=ExecutablePath "
                         "cs1={} cs2Label=CommandLine cs2={} "
                         "cs3Label=ParentPid cs3={}",
                         escapeCef(std::string{alert.rule}), escapeCef(std::string{alert.rule}),
                         escapeCef(std::string{alert.severity}), escapeCef(name), escapeCef(pid), escapeCef(path),
                         escapeCef(cmdline), escapeCef(ppid));
   }

   std::string formatLeef(const vigil::Alert& alert) {
      const auto json = nlohmann::json::parse(alert.toJson());
      const auto pid = json.at("process").at("pid").dump();
      const auto ppid = json.at("process").at("ppid").dump();
      const auto name = jsonString(json, "/process/name");
      const auto path = jsonString(json, "/process/executable_path");
      const auto cmdline = jsonString(json, "/process/command_line");

      return std::format("LEEF:2.0|vigil|vigil|1|{}|sev={}\tusrName={}\tprocName={}"
                         "\tpid={}\tppid={}\tfilePath={}\tcmdLine={}",
                         escapeLeef(std::string{alert.rule}), escapeLeef(std::string{alert.severity}), escapeLeef(name),
                         escapeLeef(name), escapeLeef(pid), escapeLeef(ppid), escapeLeef(path), escapeLeef(cmdline));
   }

   class SyslogSink final : public vigil::AlertSink {
   public:
      explicit SyslogSink(vigil::SyslogAlertFormat format)
          : format_{format} {}

      void emit(const vigil::Alert& alert) override {
         const auto line = format_ == vigil::SyslogAlertFormat::Cef ? formatCef(alert) : formatLeef(alert);
         vigil::log::warn("[SYSLOG] {}", line);
      }

   private:
      vigil::SyslogAlertFormat format_;
   };

   std::unique_ptr<vigil::AlertSink> createSink(const vigil::AlertSinkConfig& config) {
      switch (config.type) {
         case vigil::AlertSinkType::Stdout:
            return std::make_unique<StdoutSink>();
         case vigil::AlertSinkType::JsonFile:
            return std::make_unique<JsonFileSink>(config.path);
         case vigil::AlertSinkType::Syslog:
            return std::make_unique<SyslogSink>(config.format);
      }

      throw vigil::ConfigError{"unknown alert sink type"};
   }
} // namespace

namespace vigil {
   AlertDispatcher::AlertDispatcher(std::vector<std::unique_ptr<AlertSink>> sinks)
       : sinks_{std::move(sinks)} {}

   void AlertDispatcher::emit(const Alert& alert) {
      for (const auto& sink : sinks_) {
         sink->emit(alert);
      }
   }

   std::unique_ptr<AlertDispatcher> createAlertDispatcher(const Config& config) {
      std::vector<std::unique_ptr<AlertSink>> sinks;

      for (const auto& sinkConfig : config.alertSinks()) {
         if (sinkConfig.enabled)
            sinks.push_back(createSink(sinkConfig));
      }

      return std::make_unique<AlertDispatcher>(std::move(sinks));
   }
} // namespace vigil
