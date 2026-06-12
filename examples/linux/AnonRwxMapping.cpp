/**
 * @file AnonRwxMapping.cpp
 * @brief Simulates shellcode-staging behavior by allocating anonymous RWX
 * memory.
 *
 * The program creates a private anonymous memory mapping with read, write, and
 * execute permissions, writes a marker into it, and sleeps long enough for the
 * agent to observe the process. It does not execute payload bytes.
 */

#include "Common.hpp"

#include <cstring>
#include <sys/mman.h>

int main() {
  void *memory = ::mmap(nullptr, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (memory == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  std::memcpy(memory, "VIGIL", 5);
  std::cout << "allocated anonymous RWX mapping at " << memory << "\n";
  vigil::examples::sleep_for_agent();
  ::munmap(memory, 4096);
  return 0;
}
