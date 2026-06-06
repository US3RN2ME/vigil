/**
 * @file PtraceInjectRule.cpp
 * @brief Simulates process injection telemetry using PTRACE_ATTACH.
 *
 * The program starts a child process and attaches to it with ptrace. It does
 * not modify memory or execute injected code, but it produces the attach event
 * expected by ptrace injection detections.
 */

#include "Common.hpp"

#include <sys/ptrace.h>

int main() {
  std::cout << "vigil ptrace_inject_rule benign attach simulation\n";
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
  if (::ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) != 0) {
    perror("ptrace(PTRACE_ATTACH)");
    return vigil::examples::wait_for(pid);
  }

  vigil::examples::wait_for(pid);
  ::ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
  return 0;
}
