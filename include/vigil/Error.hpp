
#ifndef VIGIL_ERROR_HPP
#define VIGIL_ERROR_HPP

#include <format>
#include <stdexcept>

namespace vigil {
   struct Error : std::runtime_error {
      using std::runtime_error::runtime_error;

      template <class... Args>
      explicit Error(std::format_string<Args...> fmt, Args&&... args)
          : std::runtime_error{std::format(fmt, std::forward<Args>(args)...)} {}
   };

   struct CollectorError : Error {
      using Error::Error;
   };

   struct ConfigError : Error {
      using Error::Error;
   };
} // namespace vigil

#endif // VIGIL_ERROR_HPP
