/**
 * @file DebugPrivilegeRule.cpp
 * @brief Simulates a process enabling SeDebugPrivilege.
 *
 * The program attempts to enable SeDebugPrivilege on its own token, matching
 * the telemetry shape of debuggers, dumpers, and credential-access tooling.
 */

#include "Common.hpp"

int wmain() {
   HANDLE token = nullptr;
   if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
      std::wcerr << L"OpenProcessToken failed: " << ::GetLastError() << L"\n";
      return 1;
   }

   TOKEN_PRIVILEGES privileges{};
   privileges.PrivilegeCount = 1;
   privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
   if (!::LookupPrivilegeValueW(nullptr, L"SeDebugPrivilege", &privileges.Privileges[0].Luid)) {
      std::wcerr << L"LookupPrivilegeValueW failed: " << ::GetLastError() << L"\n";
      ::CloseHandle(token);
      return 1;
   }

   ::AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(privileges), nullptr, nullptr);
   const auto error = ::GetLastError();
   ::CloseHandle(token);

   std::wcout << L"vigil debug_privilege_rule SeDebugPrivilege simulation, "
                 L"AdjustTokenPrivileges error="
              << error << L"\n";
   vigil::examples::sleep_for_agent();
   return error == ERROR_SUCCESS ? 0 : 1;
}
