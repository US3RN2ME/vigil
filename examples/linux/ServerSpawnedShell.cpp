/**
 * @file ServerSpawnedShell.cpp
 * @brief Simulates command injection where a server worker spawns a shell.
 *
 * The process sets its comm name to nginx and forks /bin/sh to run harmless
 * discovery commands. This creates the parent/child telemetry shape of a web
 * server spawning an interactive shell.
 */

#include <sys/prctl.h>

#include "Common.hpp"

int main() {
   // Benign analogue of a web worker handling a command-injection payload.
   // The rule expects a known server process name spawning a shell.
   ::prctl(PR_SET_NAME, "nginx", 0, 0, 0);

   const pid_t pid = ::fork();
   if (pid == 0) {
      ::execl("/bin/sh", "sh", "-c", "echo vigil server_spawned_shell simulation; id; uname -a", nullptr);
      perror("execl");
      return 1;
   }
   if (pid < 0) {
      perror("fork");
      return 1;
   }

   return vigil::examples::wait_for(pid);
}
