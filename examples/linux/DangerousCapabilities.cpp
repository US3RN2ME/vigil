/**
 * @file DangerousCapabilities.cpp
 * @brief Simulates execution of a binary carrying dangerous Linux capabilities.
 *
 * The program prints its effective capability mask and stays alive briefly. To
 * trigger the rule, assign a capability such as CAP_SYS_PTRACE to this binary
 * with setcap before running it as a non-root user.
 */

#include "Common.hpp"

#include <fstream>

int main() {
  std::cout << "vigil dangerous_capabilities capability-bearing binary "
               "simulation\n";
  std::ifstream status("/proc/self/status");
  std::string line;
  while (std::getline(status, line)) {
    if (line.rfind("CapEff:", 0) == 0)
      std::cout << line << "\n";
  }

  std::cout << "To trigger this rule, build this file and run:\n"
            << "  sudo setcap cap_sys_ptrace+ep ./dangerous_capabilities\n"
            << "  ./dangerous_capabilities\n"
            << "  sudo setcap -r ./dangerous_capabilities\n";
  vigil::examples::sleep_for_agent();
  return 0;
}
