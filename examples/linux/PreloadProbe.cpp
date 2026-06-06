/**
 * @file PreloadProbe.cpp
 * @brief Benign shared object loaded by the LD_PRELOAD hijack simulator.
 *
 * The constructor records a local marker in /tmp/vigil-preload-probe.log so the
 * preload path is realistic without altering process behavior or loading
 * malicious code.
 */

#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <unistd.h>

namespace {

__attribute__((constructor)) void preload_probe_constructor() {
  const auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::array<char, 64> timestamp{};
  std::strftime(timestamp.data(), timestamp.size(), "%F %T",
                std::localtime(&now));

  FILE *file = std::fopen("/tmp/vigil-preload-probe.log", "a");
  if (file == nullptr)
    return;

  std::fprintf(file, "%s benign LD_PRELOAD probe loaded in pid=%ld uid=%ld\n",
               timestamp.data(), static_cast<long>(::getpid()),
               static_cast<long>(::getuid()));
  std::fclose(file);
}

} // namespace
