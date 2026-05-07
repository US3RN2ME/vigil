
#include "ProcessSnapshot.hpp"

#include <ntstatus.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <windows.h>
#include <winternl.h>

#include <vigil/Logger.hpp>

namespace vigil::platform::windows {

   // Forward-declare so anonymous helpers below can call it.
   std::optional<std::string> readProcessName(uint32_t pid);

   namespace {

      // ProcessCommandLineInformation is undocumented (class 60) but stable since Win8.1.
      inline const auto kProcCmdLine = static_cast<PROCESSINFOCLASS>(60);

      struct HandleGuard {
         HANDLE h{INVALID_HANDLE_VALUE};
         explicit HandleGuard(HANDLE h)
             : h(h) {}
         ~HandleGuard() {
            if (h && h != INVALID_HANDLE_VALUE)
               CloseHandle(h);
         }
         operator HANDLE() const {
            return h;
         }
         explicit operator bool() const {
            return h && h != INVALID_HANDLE_VALUE;
         }
         HandleGuard(const HandleGuard&) = delete;
         HandleGuard& operator=(const HandleGuard&) = delete;
      };

      std::string wideToUtf8(const wchar_t* wide, int wchars) {
         if (!wide || wchars <= 0)
            return {};
         const int n = WideCharToMultiByte(CP_UTF8, 0, wide, wchars, nullptr, 0, nullptr, nullptr);
         if (n <= 0)
            return {};
         std::string s(n, '\0');
         WideCharToMultiByte(CP_UTF8, 0, wide, wchars, s.data(), n, nullptr, nullptr);
         return s;
      }

      bool readExePath(ProcessInfo& p, HANDLE hProc) {
         wchar_t buf[MAX_PATH + 1]{};
         DWORD len = MAX_PATH;
         if (!QueryFullProcessImageNameW(hProc, 0, buf, &len))
            return false;
         p.exePath = wideToUtf8(buf, static_cast<int>(len));
         const auto sep = p.exePath.find_last_of("\\/");
         p.name = (sep != std::string::npos) ? p.exePath.substr(sep + 1) : p.exePath;
         return true;
      }

      void readBasicInfo(ProcessInfo& p, HANDLE hProc) {
         PROCESS_BASIC_INFORMATION pbi{};
         ULONG retLen = 0;
         if (!NT_SUCCESS(NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(pbi), &retLen)))
            return;
         // Reserved3 == InheritedFromUniqueProcessId in the semi-documented struct layout.
         p.ppid = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pbi.Reserved3));
         if (auto name = readProcessName(p.ppid))
            p.parentName = std::move(*name);
      }

      void readCmdline(ProcessInfo& p, HANDLE hProc) {
         ULONG needed = 0;
         NTSTATUS st = NtQueryInformationProcess(hProc, kProcCmdLine, nullptr, 0, &needed);
         if (st != STATUS_INFO_LENGTH_MISMATCH || needed == 0)
            return;
         std::vector<BYTE> buf(needed);
         st = NtQueryInformationProcess(hProc, kProcCmdLine, buf.data(), needed, &needed);
         if (!NT_SUCCESS(st))
            return;
         // Kernel writes a UNICODE_STRING whose Buffer pointer is patched to point
         // into our buf, followed by the wide-char data.
         auto* us = reinterpret_cast<UNICODE_STRING*>(buf.data());
         if (us->Buffer && us->Length > 0)
            p.cmdline = wideToUtf8(us->Buffer, us->Length / sizeof(wchar_t));
      }

      void readMemoryInfo(ProcessInfo& p, HANDLE hProc) {
         PROCESS_MEMORY_COUNTERS pmc{.cb = sizeof(pmc)};
         if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) {
            p.rssBytes = pmc.WorkingSetSize;
            p.vszBytes = pmc.PagefileUsage;
         }
      }

      void readThreadCount(ProcessInfo& p) {
         HandleGuard snap{CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)};
         if (!snap)
            return;
         THREADENTRY32 te{.dwSize = sizeof(te)};
         uint32_t count = 0;
         if (Thread32First(snap, &te)) {
            do {
               if (te.th32OwnerProcessID == p.pid)
                  ++count;
            } while (Thread32Next(snap, &te));
         }
         p.threadCount = count;
      }

      void readPrivileges(ProcessInfo& p, HANDLE hProc) {
         HANDLE raw = nullptr;
         if (!OpenProcessToken(hProc, TOKEN_QUERY, &raw))
            return;
         HandleGuard tok{raw};

         DWORD needed = 0;
         GetTokenInformation(tok, TokenPrivileges, nullptr, 0, &needed);
         if (!needed)
            return;
         std::vector<BYTE> buf(needed);
         if (!GetTokenInformation(tok, TokenPrivileges, buf.data(), needed, &needed))
            return;

         auto* tp = reinterpret_cast<TOKEN_PRIVILEGES*>(buf.data());
         uint64_t mask = 0;
         for (DWORD i = 0; i < tp->PrivilegeCount; ++i) {
            const DWORD luid = tp->Privileges[i].Luid.LowPart;
            if (luid < 64)
               mask |= (1ULL << luid);
         }
         p.privilegeMask = mask;
      }

      void readIntegrity(ProcessInfo& p, HANDLE hProc) {
         HANDLE raw = nullptr;
         if (!OpenProcessToken(hProc, TOKEN_QUERY, &raw))
            return;
         HandleGuard tok{raw};

         DWORD needed = 0;
         GetTokenInformation(tok, TokenIntegrityLevel, nullptr, 0, &needed);
         if (!needed)
            return;
         std::vector<BYTE> buf(needed);
         if (!GetTokenInformation(tok, TokenIntegrityLevel, buf.data(), needed, &needed))
            return;

         auto* til = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(buf.data());
         const DWORD rid = *GetSidSubAuthority(til->Label.Sid, *GetSidSubAuthorityCount(til->Label.Sid) - 1);

         if (rid < SECURITY_MANDATORY_MEDIUM_RID)
            p.integrity = ProcessInfo::Integrity::Low;
         else if (rid < SECURITY_MANDATORY_HIGH_RID)
            p.integrity = ProcessInfo::Integrity::Medium;
         else if (rid < SECURITY_MANDATORY_SYSTEM_RID)
            p.integrity = ProcessInfo::Integrity::High;
         else
            p.integrity = ProcessInfo::Integrity::System;
      }

      void readAnonRwx(ProcessInfo& p, HANDLE hProc) {
         MEMORY_BASIC_INFORMATION mbi{};
         uintptr_t addr = 0;
         while (VirtualQueryEx(hProc, reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi))) {
            if (mbi.RegionSize == 0)
               break;
            if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE &&
                (mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY)) {
               p.hasAnonRwx = true;
               break;
            }
            addr += mbi.RegionSize;
         }
      }

      void readStartTime(ProcessInfo& p, HANDLE hProc) {
         FILETIME create{}, exit{}, kernel{}, user{};
         if (!GetProcessTimes(hProc, &create, &exit, &kernel, &user))
            return;
         const ULONGLONG createFt = (static_cast<ULONGLONG>(create.dwHighDateTime) << 32) | create.dwLowDateTime;

         // Approximate boot time as: now - uptime.  Small clock-drift error is acceptable.
         FILETIME now{};
         GetSystemTimeAsFileTime(&now);
         const ULONGLONG nowFt = (static_cast<ULONGLONG>(now.dwHighDateTime) << 32) | now.dwLowDateTime;
         const ULONGLONG bootFt = nowFt - GetTickCount64() * 10'000ULL; // ms → 100ns intervals

         if (createFt >= bootFt)
            p.startTimeNs = (createFt - bootFt) * 100ULL; // 100ns intervals → nanoseconds
      }

      void readJobObject(ProcessInfo& p, HANDLE hProc) {
         BOOL inJob = FALSE;
         if (IsProcessInJob(hProc, nullptr, &inJob) && inJob)
            p.containerId = "<job>";
      }

   } // anonymous namespace

   std::optional<ProcessInfo> readProcessInfo(uint32_t pid) {
      // Try full access first; fall back to limited (works on protected processes).
      HANDLE raw = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
      if (!raw)
         raw = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
      HandleGuard hProc{raw};
      if (!hProc) {
         log::debug("process {} inaccessible or vanished", pid);
         return {};
      }

      ProcessInfo p;
      p.pid = pid;

      if (!readExePath(p, hProc)) {
         log::debug("process {} vanished before snapshot", pid);
         return {};
      }

      readBasicInfo(p, hProc);
      readCmdline(p, hProc);
      readMemoryInfo(p, hProc);
      readPrivileges(p, hProc);
      readIntegrity(p, hProc);
      readAnonRwx(p, hProc);
      readStartTime(p, hProc);
      readJobObject(p, hProc);
      readThreadCount(p);

      return p;
   }

   std::optional<std::string> readProcessName(uint32_t pid) {
      HandleGuard hProc{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)};
      if (!hProc)
         return {};
      wchar_t buf[MAX_PATH + 1]{};
      DWORD len = MAX_PATH;
      if (!QueryFullProcessImageNameW(hProc, 0, buf, &len))
         return {};
      std::string path = wideToUtf8(buf, static_cast<int>(len));
      const auto sep = path.find_last_of("\\/");
      return (sep != std::string::npos) ? path.substr(sep + 1) : path;
   }

} // namespace vigil::platform::windows
