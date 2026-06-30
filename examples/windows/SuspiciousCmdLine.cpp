/**
 * @file SuspiciousCmdLine.cpp
 * @brief Simulates encoded-command command-line telemetry.
 *
 * The program starts PowerShell with -EncodedCommand containing only a marker
 * print. This matches suspicious command-line indicators while avoiding real
 * payload behavior.
 */

#include "Common.hpp"

int wmain() {
   std::wcout << L"vigil suspicious_cmdline encoded-command simulation\n";
   return vigil::examples::run_process(vigil::examples::system32(L"WindowsPowerShell\\v1.0\\powershell.exe"),
                                       L"-NoProfile -EncodedCommand "
                                       L"VwByAGkAdABlAC0ATwB1AHQAcAB1AHQAIAAnAHYAaQBnAGkAbAAgAHMAdQBzAHAAaQBjAGk"
                                       L"AbwB1AHMAXwBjAG0AZABsAGkAbgBlACAAcwBpAG0AdQBsAGEAdABpAG8AbgAnAA==");
}
