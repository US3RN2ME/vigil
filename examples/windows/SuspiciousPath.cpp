/**
 * @file SuspiciousPath.cpp
 * @brief Simulates execution from C:/Users/Public.
 *
 * The program copies itself to a public writable staging directory and executes
 * the copy, matching common malware staging path telemetry.
 */

#include "Common.hpp"

int wmain(int argc, wchar_t** argv) {
   if (vigil::examples::has_arg(argc, argv, L"--child")) {
      std::wcout << L"vigil suspicious_path dropper-shaped simulation\n";
      vigil::examples::sleep_for_agent();
      return 0;
   }

   const auto target = vigil::examples::public_examples_dir() / L"update-helper.exe";
   vigil::examples::copy_self_to(target);
   return vigil::examples::run_process(target, L"--child");
}
