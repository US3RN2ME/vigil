
#ifndef VIGIL_PLATFORM_WINAPI_HPP
#define VIGIL_PLATFORM_WINAPI_HPP

// clang-format off
#include <winsock2.h>
#include <ws2tcpip.h>   // inet_ntop, IN6_ADDR, getaddrinfo extensions
#include <windows.h>
#include <winternl.h>   // NtQueryInformationProcess, UNICODE_STRING, PEB
#include <evntcons.h>
#include <evntrace.h>   // ETW: StartTrace, EnableTraceEx2, ProcessTrace
#include <iphlpapi.h>   // IP Helper: GetExtendedTcpTable
#include <psapi.h>      // GetProcessMemoryInfo
#include <tlhelp32.h>   // CreateToolhelp32Snapshot, PROCESSENTRY32W
// clang-format on

#endif // VIGIL_PLATFORM_WINAPI_HPP
