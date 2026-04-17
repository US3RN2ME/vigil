
#ifndef VIGIL_ERROR_HPP
#define VIGIL_ERROR_HPP

#include <stdexcept>

namespace vigil {
   class Error : public std::runtime_error {
      using std::runtime_error::runtime_error;
   };

   class CollectorError : public Error {
      using Error::Error;
   };

   class ConfigError : public Error {
      using Error::Error;
   };
} // namespace vigil

#endif // VIGIL_ERROR_HPP
