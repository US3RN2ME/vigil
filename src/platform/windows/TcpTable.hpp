
#ifndef VIGIL_PLATFORM_TCPTABLE_HPP
#define VIGIL_PLATFORM_TCPTABLE_HPP

#include <cstdint>
#include <functional>
#include <string>

namespace vigil::platform {

   struct TcpConnection {
      uint32_t pid;
      uint16_t remotePort;
      std::string remoteAddr;

      bool operator==(const TcpConnection&) const = default;
   };

   struct TcpConnectionHash {
      std::size_t operator()(const TcpConnection& connection) const noexcept {
         std::size_t hash = std::hash<uint32_t>{}(connection.pid);
         hash ^= std::hash<uint16_t>{}(connection.remotePort) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
         hash ^= std::hash<std::string>{}(connection.remoteAddr) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
         return hash;
      }
   };

   class TcpTable {
   public:
      static void forEach(const std::function<void(const TcpConnection&)>& callback);
   };

} // namespace vigil::platform

#endif // VIGIL_PLATFORM_TCPTABLE_HPP
