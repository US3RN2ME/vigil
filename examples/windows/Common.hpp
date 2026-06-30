#ifndef VIGIL_EXAMPLES_WINDOWS_COMMON_HPP
#define VIGIL_EXAMPLES_WINDOWS_COMMON_HPP

#define WIN32_LEAN_AND_MEAN
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <shellapi.h>
#include <string>
#include <string_view>
#include <thread>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace vigil::examples {

   inline void sleep_for_agent(int seconds = 2) {
      std::this_thread::sleep_for(std::chrono::seconds(seconds));
   }

   inline std::filesystem::path self_path() {
      std::wstring buffer(MAX_PATH, L'\0');
      const DWORD size = ::GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
      if (size == 0) {
         std::wcerr << L"GetModuleFileNameW failed: " << ::GetLastError() << L"\n";
         std::exit(1);
      }
      buffer.resize(size);
      return buffer;
   }

   inline std::filesystem::path public_examples_dir() {
      return std::filesystem::path{L"C:\\Users\\Public\\vigil-examples"};
   }

   inline void copy_self_to(const std::filesystem::path& target) {
      std::filesystem::create_directories(target.parent_path());
      std::filesystem::copy_file(self_path(), target, std::filesystem::copy_options::overwrite_existing);
   }

   inline bool has_arg(int argc, wchar_t** argv, std::wstring_view expected) {
      for (int i = 1; i < argc; ++i) {
         if (argv[i] == expected)
            return true;
      }
      return false;
   }

   inline int run_process(const std::filesystem::path& image, const std::wstring& arguments = L"", bool wait = true) {
      std::wstring command = L"\"" + image.wstring() + L"\"";
      if (!arguments.empty())
         command += L" " + arguments;

      STARTUPINFOW startup{};
      startup.cb = sizeof(startup);
      PROCESS_INFORMATION process{};
      if (!::CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &process)) {
         std::wcerr << L"CreateProcessW failed for " << image << L": " << ::GetLastError() << L"\n";
         return 1;
      }

      if (wait)
         ::WaitForSingleObject(process.hProcess, INFINITE);
      ::CloseHandle(process.hThread);
      ::CloseHandle(process.hProcess);
      return 0;
   }

   inline int run_elevated(const std::filesystem::path& image, const std::wstring& arguments = L"") {
      SHELLEXECUTEINFOW info{};
      info.cbSize = sizeof(info);
      info.fMask = SEE_MASK_NOCLOSEPROCESS;
      info.lpVerb = L"runas";
      info.lpFile = image.c_str();
      info.lpParameters = arguments.c_str();
      info.nShow = SW_SHOWNORMAL;

      if (!::ShellExecuteExW(&info)) {
         std::wcerr << L"ShellExecuteExW(runas) failed: " << ::GetLastError() << L"\n";
         return 1;
      }

      if (info.hProcess != nullptr) {
         ::WaitForSingleObject(info.hProcess, INFINITE);
         ::CloseHandle(info.hProcess);
      }
      return 0;
   }

   inline std::filesystem::path system32(std::wstring_view name) {
      wchar_t root[MAX_PATH]{};
      ::GetSystemDirectoryW(root, MAX_PATH);
      return std::filesystem::path{root} / name;
   }

} // namespace vigil::examples

#endif // VIGIL_EXAMPLES_WINDOWS_COMMON_HPP
