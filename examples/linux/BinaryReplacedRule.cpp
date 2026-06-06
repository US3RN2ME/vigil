/**
 * @file BinaryReplacedRule.cpp
 * @brief Simulates a self-updater or masquerading binary replacement after
 * exec.
 *
 * The program copies itself to a hidden temporary helper path, executes that
 * image, then atomically replaces the on-disk image while the child is still
 * running. This mirrors malware-like backing-file replacement telemetry without
 * destructive behavior.
 */

#include "Common.hpp"

int main(int argc, char **argv) {
  const std::filesystem::path target = "/tmp/.vigil-cache/self-updater";

  if (vigil::examples::has_arg(argc, argv, "--child")) {
    std::cout << "vigil binary_replaced_rule self-update simulation\n";
    vigil::examples::sleep_for_agent(4);
    return 0;
  }

  vigil::examples::copy_self_to(target);
  const pid_t pid = ::fork();
  if (pid == 0)
    vigil::examples::exec_path(target, {"--child"});
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  const std::filesystem::path replacement =
      "/tmp/.vigil-cache/self-updater.new";
  std::filesystem::copy_file("/bin/true", replacement,
                             std::filesystem::copy_options::overwrite_existing);
  std::filesystem::rename(replacement, target);
  return vigil::examples::wait_for(pid);
}
