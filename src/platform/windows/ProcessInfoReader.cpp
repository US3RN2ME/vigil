#include "ProcessInfoReader.hpp"

#include <cstdint>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <windows.h>
#include <winternl.h>

#include "Handle.hpp"
#include "Process.hpp"
#include "ProcessToken.hpp"
#include "StringUtils.hpp"

#include <vigil/Logger.hpp>

namespace vigil::platform::windows {
   namespace {

      inline const auto kProcessCommandLineInformation = static_cast<PROCESSINFOCLASS>(60);
      constexpr NTSTATUS kStatusInfoLengthMismatch = static_cast<NTSTATUS>(0xC0000004L);

      constexpr bool ntSuccess(NTSTATUS status) noexcept {
         return status >= 0;
      }

      bool readExePath(ProcessInfo& p, HANDLE process) {
         wchar_t buffer[MAX_PATH + 1]{};
         DWORD length = MAX_PATH;

         if (!QueryFullProcessImageNameW(process, 0, buffer, &length))
            return false;

         p.exePath = StringUtils::wideToUtf8(buffer, static_cast<int>(length));
         p.name = StringUtils::filenameFromPath(p.exePath);

         return true;
      }

      void readBasicInfo(ProcessInfo& p, HANDLE process, const ProcessInfoReader& reader) {
         PROCESS_BASIC_INFORMATION pbi{};
         ULONG returnedLength = 0;

         const auto status = NtQueryInformationProcess(process, ProcessBasicInformation, &pbi, sizeof(pbi), &returnedLength);

         if (!ntSuccess(status))
            return;

         p.ppid = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pbi.Reserved3));

         if (auto name = reader.readName(p.ppid))
            p.parentName = std::move(*name);
      }

      void readCmdline(ProcessInfo& p, HANDLE process) {
         ULONG needed = 0;

         NTSTATUS status = NtQueryInformationProcess(process, kProcessCommandLineInformation, nullptr, 0, &needed);

         if (status != kStatusInfoLengthMismatch || needed == 0)
            return;

         std::vector<std::byte> buffer(needed);

         status = NtQueryInformationProcess(process, kProcessCommandLineInformation, buffer.data(), needed, &needed);

         if (!ntSuccess(status))
            return;

         const auto* unicode = reinterpret_cast<const UNICODE_STRING*>(buffer.data());

         if (unicode->Buffer && unicode->Length > 0) {
            p.cmdline = StringUtils::wideToUtf8(unicode->Buffer, unicode->Length / sizeof(wchar_t));
         }
      }

      void readMemoryInfo(ProcessInfo& p, HANDLE process) {
         PROCESS_MEMORY_COUNTERS counters{};
         counters.cb = sizeof(counters);

         if (!GetProcessMemoryInfo(process, &counters, sizeof(counters)))
            return;

         p.rssBytes = counters.WorkingSetSize;
         p.vszBytes = counters.PagefileUsage;
      }

      void readThreadCount(ProcessInfo& p) {
         Handle snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)};

         if (!snapshot)
            return;

         THREADENTRY32 entry{};
         entry.dwSize = sizeof(entry);

         uint32_t count = 0;

         if (Thread32First(static_cast<HANDLE>(snapshot.native()), &entry)) {
            do {
               if (entry.th32OwnerProcessID == p.pid)
                  ++count;
            } while (Thread32Next(static_cast<HANDLE>(snapshot.native()), &entry));
         }

         p.threadCount = count;
      }

      void readPrivileges(ProcessInfo& p, HANDLE process) {
         auto token = ProcessToken::open(process);
         if (!token)
            return;

         auto buffer = token->query(ProcessToken::InfoClass::Privileges);
         if (buffer.empty())
            return;

         const auto* privileges = reinterpret_cast<const TOKEN_PRIVILEGES*>(buffer.data());

         uint64_t mask = 0;

         for (DWORD i = 0; i < privileges->PrivilegeCount; ++i) {
            const DWORD luid = privileges->Privileges[i].Luid.LowPart;

            if (luid < 64)
               mask |= 1ULL << luid;
         }

         p.privilegeMask = mask;
      }

      void readIntegrity(ProcessInfo& p, HANDLE process) {
         auto token = ProcessToken::open(process);
         if (!token)
            return;

         auto buffer = token->query(ProcessToken::InfoClass::IntegrityLevel);
         if (buffer.empty())
            return;

         const auto* label = reinterpret_cast<const TOKEN_MANDATORY_LABEL*>(buffer.data());

         const DWORD rid = *GetSidSubAuthority(label->Label.Sid, *GetSidSubAuthorityCount(label->Label.Sid) - 1);

         if (rid < SECURITY_MANDATORY_MEDIUM_RID)
            p.integrity = ProcessInfo::Integrity::Low;
         else if (rid < SECURITY_MANDATORY_HIGH_RID)
            p.integrity = ProcessInfo::Integrity::Medium;
         else if (rid < SECURITY_MANDATORY_SYSTEM_RID)
            p.integrity = ProcessInfo::Integrity::High;
         else
            p.integrity = ProcessInfo::Integrity::System;
      }

      void readAnonRwx(ProcessInfo& p, HANDLE process) {
         MEMORY_BASIC_INFORMATION mbi{};
         uintptr_t address = 0;

         while (VirtualQueryEx(process, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
            if (mbi.RegionSize == 0)
               break;

            const bool isRwx = mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_WRITECOPY;

            if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE && isRwx) {
               p.hasAnonRwx = true;
               break;
            }

            address += mbi.RegionSize;
         }
      }

      void readStartTime(ProcessInfo& p, HANDLE process) {
         FILETIME create{};
         FILETIME exit{};
         FILETIME kernel{};
         FILETIME user{};

         if (!GetProcessTimes(process, &create, &exit, &kernel, &user))
            return;

         const ULONGLONG createFileTime = (static_cast<ULONGLONG>(create.dwHighDateTime) << 32) | create.dwLowDateTime;

         FILETIME now{};
         GetSystemTimeAsFileTime(&now);

         const ULONGLONG nowFileTime = (static_cast<ULONGLONG>(now.dwHighDateTime) << 32) | now.dwLowDateTime;

         const ULONGLONG bootFileTime = nowFileTime - GetTickCount64() * 10'000ULL;

         if (createFileTime >= bootFileTime)
            p.startTimeNs = (createFileTime - bootFileTime) * 100ULL;
      }

      void readJobObject(ProcessInfo& p, HANDLE process) {
         BOOL inJob = FALSE;

         if (IsProcessInJob(process, nullptr, &inJob) && inJob)
            p.containerId = "<job>";
      }

   } // namespace

   std::optional<ProcessInfo> ProcessInfoReader::read(uint32_t pid) const {
      auto process = Process::open(pid);

      if (!process) {
         log::debug("process {} inaccessible or vanished", pid);
         return {};
      }

      ProcessInfo info;
      info.pid = pid;

      const auto nativeProcess = static_cast<HANDLE>(process.native());

      if (!readExePath(info, nativeProcess)) {
         log::debug("process {} vanished before snapshot", pid);
         return std::nullopt;
      }

      readBasicInfo(info, nativeProcess, *this);
      readCmdline(info, nativeProcess);
      readMemoryInfo(info, nativeProcess);
      readPrivileges(info, nativeProcess);
      readIntegrity(info, nativeProcess);
      readAnonRwx(info, nativeProcess);
      readStartTime(info, nativeProcess);
      readJobObject(info, nativeProcess);
      readThreadCount(info);

      return info;
   }

   std::optional<std::string> ProcessInfoReader::readName(uint32_t pid) const {
      auto process = Process::open(pid);

      if (!process)
         return std::nullopt;

      wchar_t buffer[MAX_PATH + 1]{};
      DWORD length = MAX_PATH;

      if (!QueryFullProcessImageNameW(static_cast<HANDLE>(process.native()), 0, buffer, &length)) {
         return std::nullopt;
      }

      auto path = StringUtils::wideToUtf8(buffer, static_cast<int>(length));

      return StringUtils::filenameFromPath(path);
   }

} // namespace vigil::platform::windows
