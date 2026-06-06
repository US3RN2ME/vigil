
#include "WinApi.hpp"

#include <vigil/SystemError.hpp>

namespace vigil::error {
int lastCode() { return static_cast<int>(GetLastError()); }
} // namespace vigil::error
