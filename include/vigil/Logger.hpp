
#ifndef VIGIL_LOGGER_HPP
#define VIGIL_LOGGER_HPP

#include <format>
#include <string_view>

namespace vigil::log {
   void debug(std::string_view msg);
   void info(std::string_view msg);
   void warn(std::string_view msg);
   void error(std::string_view msg);

   template <class... Args>
   void debug(std::format_string<Args...> fmt, Args&&... args) {
      debug(std::format(fmt, std::forward<Args>(args)...));
   }

   template <class... Args>
   void info(std::format_string<Args...> fmt, Args&&... args) {
      info(std::format(fmt, std::forward<Args>(args)...));
   }

   template <class... Args>
   void warn(std::format_string<Args...> fmt, Args&&... args) {
      warn(std::format(fmt, std::forward<Args>(args)...));
   }

   template <class... Args>
   void error(std::format_string<Args...> fmt, Args&&... args) {
      error(std::format(fmt, std::forward<Args>(args)...));
   }

   void init(std::string_view logFile = "");
} // namespace vigil::log

#endif // VIGIL_LOGGER_HPP
