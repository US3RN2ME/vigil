/**
 * @file ElevatedSuspiciousPath.cpp
 * @brief Simulates a UAC-elevated helper launched from a public path.
 *
 * The program copies itself to C:/Users/Public/vigil-examples and relaunches
 * it with the runas verb, creating high-integrity execution from a suspicious
 * writable location.
 */

#include "Common.hpp"

int wmain(int argc, wchar_t **argv) {
  if (vigil::examples::has_arg(argc, argv, L"--child")) {
    std::wcout << L"vigil elevated_suspicious_path UAC helper simulation\n";
    vigil::examples::sleep_for_agent();
    return 0;
  }

  const auto target =
      vigil::examples::public_examples_dir() / L"elevated-helper.exe";
  vigil::examples::copy_self_to(target);
  return vigil::examples::run_elevated(target, L"--child");
}
