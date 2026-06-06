/**
 * @file PrivilegeEscalationRule.cpp
 * @brief Alias simulator for the Linux privilege_escalation_rule config key.
 *
 * This program mirrors privilege_escalation.cpp so both Linux configuration
 * aliases can be exercised with separate binaries.
 */

#include "Common.hpp"

int main() {
  std::cout << "uid=" << ::getuid() << " euid=" << ::geteuid() << "\n";
  std::cout << "Linux config alias for privilege_escalation\n";
  ::execlp("sudo", "sudo", "id", "-u", nullptr);
  perror("execlp(sudo)");
  return 1;
}
