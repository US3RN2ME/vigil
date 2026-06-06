/**
 * @file SuspiciousPort.cpp
 * @brief Simulates beacon traffic to a suspicious TCP port.
 *
 * The program attempts a TCP connection to 127.0.0.1:4444 and sends a harmless
 * HTTP-like beacon if a local listener is present.
 */

#include "Common.hpp"

int wmain() {
  WSADATA data{};
  if (::WSAStartup(MAKEWORD(2, 2), &data) != 0)
    return 1;

  SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(4444);
  ::InetPtonW(AF_INET, L"127.0.0.1", &addr.sin_addr);

  std::wcout << L"attempting beacon-shaped connect to 127.0.0.1:4444\n";
  if (::connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0) {
    const char beacon[] =
        "GET /beacon?id=vigil-windows-example HTTP/1.0\r\n\r\n";
    ::send(sock, beacon, static_cast<int>(sizeof(beacon) - 1), 0);
  }

  ::closesocket(sock);
  ::WSACleanup();
  vigil::examples::sleep_for_agent();
  return 0;
}
