
#include <cerrno>

#include <vigil/SystemError.hpp>

namespace vigil::error {
   int lastCode() {
      return errno;
   }
} // namespace vigil::error
