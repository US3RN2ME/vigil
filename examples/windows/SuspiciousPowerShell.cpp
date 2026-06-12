/**
 * @file SuspiciousPowerShell.cpp
 * @brief Simulates hidden encoded PowerShell execution.
 *
 * The program launches PowerShell with -WindowStyle Hidden and -EncodedCommand
 * containing only a marker print, matching suspicious PowerShell telemetry.
 */

#include "Common.hpp"

int wmain() {
  std::wcout << L"vigil suspicious_powershell hidden encoded simulation\n";
  return vigil::examples::run_process(
      vigil::examples::system32(L"WindowsPowerShell\\v1.0\\powershell.exe"),
      L"-NoProfile -WindowStyle Hidden -EncodedCommand "
      L"VwByAGkAdABlAC0ATwB1AHQAcAB1AHQAIAAnAHYAaQBnAGkAbAAgAHMAdQBzAHAAaQBjAGk"
      L"AbwB1AHMAXwBwAG8AdwBlAHIAcwBoAGUAbABsACAAcwBpAG0AdQBsAGEAdABpAG8AbgAnAA"
      L"==");
}
