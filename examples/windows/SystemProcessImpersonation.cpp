/**
 * @file SystemProcessImpersonation.cpp
 * @brief Simulates protected Windows process-name impersonation.
 *
 * The program copies itself as lsass.exe outside System32 and executes it,
 * matching the telemetry shape of malware masquerading as a protected system
 * process.
 */

#include "Common.hpp"

int wmain(int argc, wchar_t **argv) {
  if (vigil::examples::has_arg(argc, argv, L"--child")) {
    std::wcout << L"vigil system_process_impersonation protected-name "
                  L"simulation\n";
    vigil::examples::sleep_for_agent();
    return 0;
  }

  const auto target = vigil::examples::public_examples_dir() / L"lsass.exe";
  vigil::examples::copy_self_to(target);
  return vigil::examples::run_process(target, L"--child");
}
