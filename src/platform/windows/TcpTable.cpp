#include "TcpTable.hpp"

#include <optional>
#include <vector>

#include "WinApi.hpp"

namespace vigil::platform {

namespace {

bool isLoopback4(DWORD addr) {
  // dwRemoteAddr is in network byte order; first octet is the low byte.
  return (addr & 0xFF) == 127;
}

bool isLoopback6(const UCHAR (&addr)[16]) {
  static constexpr UCHAR kLoopback[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0, 0, 1};
  return memcmp(addr, kLoopback, 16) == 0;
}

template <typename Table, typename Row, typename Extractor>
void scanTcp(ADDRESS_FAMILY family,
             const std::function<void(const TcpConnection &)> &callback,
             Extractor &&extractor) {
  ULONG size = 0;

  const auto result = GetExtendedTcpTable(nullptr, &size, FALSE, family,
                                          TCP_TABLE_OWNER_PID_ALL, 0);

  if (result != ERROR_INSUFFICIENT_BUFFER)
    return;

  std::vector<std::byte> buffer(size);

  if (GetExtendedTcpTable(buffer.data(), &size, FALSE, family,
                          TCP_TABLE_OWNER_PID_ALL, 0) != NO_ERROR)
    return;

  const auto *table = reinterpret_cast<const Table *>(buffer.data());

  for (DWORD i = 0; i < table->dwNumEntries; ++i) {
    const Row &row = table->table[i];

    if (row.dwState != MIB_TCP_STATE_ESTAB)
      continue;

    if (auto conn = extractor(row))
      callback(*conn);
  }
}

std::optional<TcpConnection> extractIPv4(const MIB_TCPROW_OWNER_PID &row) {
  if (isLoopback4(row.dwRemoteAddr))
    return {};

  char addr[INET_ADDRSTRLEN]{};

  IN_ADDR in{};
  in.S_un.S_addr = row.dwRemoteAddr;

  if (!inet_ntop(AF_INET, &in, addr, sizeof(addr)))
    return {};

  return TcpConnection{
      .pid = row.dwOwningPid,
      .remotePort = ntohs(static_cast<u_short>(row.dwRemotePort)),
      .remoteAddr = addr,
  };
}

std::optional<TcpConnection> extractIPv6(const MIB_TCP6ROW_OWNER_PID &row) {
  if (isLoopback6(row.ucRemoteAddr))
    return {};

  char addr[INET6_ADDRSTRLEN]{};

  IN6_ADDR in6{};
  std::memcpy(&in6, row.ucRemoteAddr, sizeof(in6));

  if (!inet_ntop(AF_INET6, &in6, addr, sizeof(addr)))
    return {};

  return TcpConnection{
      .pid = row.dwOwningPid,
      .remotePort = ntohs(static_cast<u_short>(row.dwRemotePort)),
      .remoteAddr = addr,
  };
}

} // namespace

void TcpTable::forEach(
    const std::function<void(const TcpConnection &)> &callback) {
  scanTcp<MIB_TCPTABLE_OWNER_PID, MIB_TCPROW_OWNER_PID>(AF_INET, callback,
                                                        extractIPv4);
  scanTcp<MIB_TCP6TABLE_OWNER_PID, MIB_TCP6ROW_OWNER_PID>(AF_INET6, callback,
                                                          extractIPv6);
}

} // namespace vigil::platform
