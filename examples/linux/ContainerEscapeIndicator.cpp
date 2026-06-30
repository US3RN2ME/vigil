/**
 * @file ContainerEscapeIndicator.cpp
 * @brief Simulates a container-escape-relevant ptrace action.
 *
 * The program attaches to a child process with ptrace to generate telemetry
 * associated with debugger or injection activity. In a container this resembles
 * one observable signal used for escape detection.
 */

#include <sys/ptrace.h>

#include "Common.hpp"

int main() {
   std::cout << "vigil container_escape_indicator ptrace-shaped simulation\n";
   const pid_t pid = ::fork();
   if (pid == 0) {
      vigil::examples::sleep_for_agent(5);
      return 0;
   }
   if (pid < 0) {
      perror("fork");
      return 1;
   }

   std::this_thread::sleep_for(std::chrono::milliseconds(200));
   if (::ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) == 0) {
      vigil::examples::wait_for(pid);
      ::ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
   } else {
      perror("ptrace(PTRACE_ATTACH)");
   }

   return vigil::examples::wait_for(pid);
}
