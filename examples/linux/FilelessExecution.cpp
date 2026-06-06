/**
 * @file FilelessExecution.cpp
 * @brief Simulates fileless execution using memfd_create and fexecve.
 *
 * The program copies its own ELF image into an anonymous memfd and executes it
 * without a normal filesystem path. This creates realistic memfd execution
 * telemetry while running only the same benign simulator code.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "Common.hpp"

#include <array>
#include <fstream>
#include <sys/mman.h>

extern char **environ;

int main(int argc, char **argv) {
  if (vigil::examples::has_arg(argc, argv, "--memfd-child")) {
    std::cout << "vigil fileless_execution benign memfd payload\n";
    vigil::examples::sleep_for_agent(4);
    return 0;
  }

  const int fd = ::memfd_create("vigil_memfd_payload", 0);
  if (fd < 0) {
    perror("memfd_create");
    return 1;
  }

  std::ifstream input(vigil::examples::self_path(), std::ios::binary);
  if (!input) {
    std::cerr << "failed to open self image\n";
    return 1;
  }

  std::array<char, 16384> buffer{};
  while (input) {
    input.read(buffer.data(), buffer.size());
    const auto size = input.gcount();
    if (size > 0 && ::write(fd, buffer.data(), static_cast<size_t>(size)) < 0) {
      perror("write(memfd)");
      return 1;
    }
  }

  ::fchmod(fd, 0700);
  char arg0[] = "vigil_memfd_payload";
  char arg1[] = "--memfd-child";
  char *child_argv[] = {arg0, arg1, nullptr};
  ::fexecve(fd, child_argv, environ);
  perror("fexecve(memfd)");
  return 1;
}
