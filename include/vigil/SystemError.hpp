
#ifndef VIGIL_SYSTEMERROR_HPP
#define VIGIL_SYSTEMERROR_HPP

#include <string>

namespace vigil::error {
   [[nodiscard]] std::string message(int code);
   [[nodiscard]] std::string lastMessage();
   [[nodiscard]] int lastCode();
} // namespace vigil::error

#endif // VIGIL_SYSTEMERROR_HPP
