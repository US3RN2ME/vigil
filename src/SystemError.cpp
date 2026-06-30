#include <system_error>

#include <vigil/SystemError.hpp>

namespace vigil::error {
   std::string message(int code) {
      return std::error_code{code, std::system_category()}.message();
   }

   std::string lastMessage() {
      return message(lastCode());
   }
} // namespace vigil::error
