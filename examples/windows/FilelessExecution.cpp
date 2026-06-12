/**
 * @file FilelessExecution.cpp
 * @brief Simulates fileless PowerShell execution with encoded in-memory script.
 *
 * The program launches PowerShell with an EncodedCommand payload that only
 * prints a marker. The command-line shape resembles reflection or fileless
 * execution without loading malicious content.
 */

#include "Common.hpp"

int wmain() {
  std::wcout << L"vigil fileless_execution PowerShell reflection-shaped "
                L"simulation\n";
  return vigil::examples::run_process(
      vigil::examples::system32(L"WindowsPowerShell\\v1.0\\powershell.exe"),
      L"-NoProfile -ExecutionPolicy Bypass -EncodedCommand "
      L"VwByAGkAdABlAC0ATwB1AHQAcAB1AHQAIAAnAHYAaQBnAGkAbAAgAGYAaQBsAGUAbABlAHM"
      L"AcwBfAGUAeABlAGMAdQB0AGkAbwBuACAAcwBpAG0AdQBsAGEAdABpAG8AbgAnAA==");
}
