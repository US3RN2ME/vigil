/**
 * @file PrivilegeEscalation.cpp
 * @brief Simulates a sudo-based privilege escalation boundary.
 *
 * The program prints its current identity and starts a sudo child. This creates
 * before/after privilege telemetry similar to tools that cross an elevation
 * boundary.
 */

#include "Common.hpp"

int main() {
  std::cout << "uid=" << ::getuid() << " euid=" << ::geteuid() << "\n";
  std::cout << "starting a sudo child to create an elevation boundary\n";
  ::execlp("sudo", "sudo", "id", "-u", nullptr);
  perror("execlp(sudo)");
  return 1;
}
