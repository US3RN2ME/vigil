
#ifndef VIGIL_PLATFORM_WINDOWS_TCPTABLE_HPP
#define VIGIL_PLATFORM_WINDOWS_TCPTABLE_HPP

#include <cstdint>
#include <functional>
#include <string>

namespace vigil::platform::windows {

   struct TcpConnection {
      uint32_t pid;
      uint16_t remotePort;
      std::string remoteAddr;
   };

   class TcpTable {
   public:
      static void forEach(const std::function<void(const TcpConnection&)>& callback);
   };

} // namespace vigil::platform::windows

#endif // VIGIL_PLATFORM_WINDOWS_TCPTABLE_HPP
