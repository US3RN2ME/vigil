/**
 * @file SuspiciousPort.cpp
 * @brief Simulates command-and-control beacon traffic to a suspicious port.
 *
 * The program attempts a local TCP connection to 127.0.0.1:4444 and, if a
 * listener exists, sends a harmless HTTP-like beacon string.
 */

#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

#include "Common.hpp"

int main() {
   const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
   if (fd < 0) {
      perror("socket");
      return 1;
   }

   sockaddr_in addr{};
   addr.sin_family = AF_INET;
   addr.sin_port = htons(4444);
   ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

   std::cout << "attempting benign beacon-shaped TCP connect to 127.0.0.1:4444\n";
   if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
      const char beacon[] = "GET /beacon?id=vigil-example HTTP/1.0\r\n\r\n";
      (void)::send(fd, beacon, std::strlen(beacon), 0);
   }
   ::close(fd);
   vigil::examples::sleep_for_agent();
   return 0;
}
