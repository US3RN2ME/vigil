/**
 * @file PrivilegeEscalation.cpp
 * @brief Simulates crossing a Windows UAC elevation boundary.
 *
 * The program launches cmd.exe through the runas verb to produce elevated-child
 * telemetry similar to tools requesting administrator privileges.
 */

#include "Common.hpp"

int wmain() {
   std::wcout << L"vigil privilege_escalation UAC boundary simulation\n";
   return vigil::examples::run_elevated(vigil::examples::system32(L"cmd.exe"), L"/c whoami /priv");
}
