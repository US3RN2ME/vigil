/**
 * @file ElevatedSuspiciousPath.cpp
 * @brief Simulates a root helper launched from a suspicious temporary path.
 *
 * The program copies itself to /tmp/.vigil-cache/root-helper and relaunches it
 * through sudo unless it is already elevated. This mirrors droppers that stage
 * privileged helpers from writable directories.
 */

#include "Common.hpp"

int main(int argc, char** argv) {
   const std::filesystem::path target = "/tmp/.vigil-cache/root-helper";

   if (vigil::examples::has_arg(argc, argv, "--child")) {
      std::cout << "vigil elevated_suspicious_path root-helper simulation, euid=" << ::geteuid() << "\n";
      vigil::examples::sleep_for_agent();
      return 0;
   }

   vigil::examples::copy_self_to(target);
   if (::geteuid() == 0)
      vigil::examples::exec_path(target, {"--child"});

   ::execlp("sudo", "sudo", target.c_str(), "--child", nullptr);
   perror("execlp(sudo)");
   return 1;
}
