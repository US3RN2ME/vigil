/**
 * @file SuspiciousPath.cpp
 * @brief Simulates execution of a staged helper from a world-writable path.
 *
 * The program copies itself to /tmp/.vigil-cache/update-helper and executes the
 * copy, matching the suspicious-path telemetry common to droppers and temporary
 * payload staging.
 */

#include "Common.hpp"

int main(int argc, char** argv) {
   if (vigil::examples::has_arg(argc, argv, "--child")) {
      std::cout << "vigil suspicious_path dropper-shaped simulation\n";
      vigil::examples::sleep_for_agent();
      return 0;
   }

   const std::filesystem::path target = "/tmp/.vigil-cache/update-helper";
   vigil::examples::copy_self_to(target);
   vigil::examples::exec_path(target, {"--child"});
}
