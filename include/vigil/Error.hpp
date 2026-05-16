
#ifndef VIGIL_ERROR_HPP
#define VIGIL_ERROR_HPP

#include <format>
#include <stdexcept>

namespace vigil {
   /**
    * @brief Base exception type for Vigil runtime failures.
    */
   struct Error : std::runtime_error {
      using std::runtime_error::runtime_error;

      /**
       * @brief Construct an Error from a std::format-compatible format string.
       *
       * @tparam Args Format
       * argument types.
       *
       * @param fmt Format string.
       * @param args Format arguments.
       */
      template <class... Args>
      explicit Error(std::format_string<Args...> fmt, Args&&... args)
          : std::runtime_error{std::format(fmt, std::forward<Args>(args)...)} {}
   };

   /**
    * @brief Event collector initialization or runtime failure.
    */
   struct CollectorError : Error {
      using Error::Error;
   };

   /**
    * @brief Configuration load or parse failure.
    */
   struct ConfigError : Error {
      using Error::Error;
   };

   /**
    * @brief OS signal or console-control handler failure.
    */
   struct SignalHandlerError : Error {
      using Error::Error;
   };
} // namespace vigil

#endif // VIGIL_ERROR_HPP
