
#ifndef VIGIL_SYSTEMERROR_HPP
#define VIGIL_SYSTEMERROR_HPP

#include <string>

namespace vigil::error {
   /**
    * @brief Format an operating-system error code as a human-readable message.
    *
    * @param code Operating-system error code.
    *
    * @return Human-readable error message.
    */
   [[nodiscard]] std::string message(int code);

   /**
    * @brief Return the message for the current thread's last OS error.
    *
    * @return Human-readable last-error message.
    */
   [[nodiscard]] std::string lastMessage();

   /**
    * @brief Return the current thread's last OS error code.
    *
    * @return Last operating-system error code for the current thread.
    */
   [[nodiscard]] int lastCode();
} // namespace vigil::error

#endif // VIGIL_SYSTEMERROR_HPP
