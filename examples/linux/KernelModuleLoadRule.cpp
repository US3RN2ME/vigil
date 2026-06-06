/**
 * @file KernelModuleLoadRule.cpp
 * @brief Simulates a guarded kernel module load attempt.
 *
 * With VIGIL_ALLOW_DANGEROUS=1, the program invokes modprobe for the loop
 * module to generate module-load telemetry. It uses an explicit opt-in guard
 * because module loading changes kernel state.
 */

#include "Common.hpp"

int main() {
  vigil::examples::require_dangerous();
  std::cout << "vigil kernel_module_load_rule benign modprobe simulation\n";
  std::cout << "invoking modprobe loop\n";
  return std::system("modprobe loop");
}
