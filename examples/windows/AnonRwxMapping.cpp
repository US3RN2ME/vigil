/**
 * @file AnonRwxMapping.cpp
 * @brief Simulates shellcode staging with anonymous RWX virtual memory.
 *
 * The program allocates a PAGE_EXECUTE_READWRITE region with VirtualAlloc and
 * sleeps for agent observation. It does not write or execute shellcode.
 */

#include "Common.hpp"

int wmain() {
  void *memory = ::VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE,
                                PAGE_EXECUTE_READWRITE);
  if (memory == nullptr) {
    std::wcerr << L"VirtualAlloc RWX failed: " << ::GetLastError() << L"\n";
    return 1;
  }

  std::wcout << L"allocated anonymous RWX memory at " << memory << L"\n";
  vigil::examples::sleep_for_agent();
  ::VirtualFree(memory, 0, MEM_RELEASE);
  return 0;
}
