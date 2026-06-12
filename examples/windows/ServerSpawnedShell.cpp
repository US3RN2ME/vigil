/**
 * @file ServerSpawnedShell.cpp
 * @brief Simulates an IIS worker process spawning cmd.exe.
 *
 * The program copies itself as w3wp.exe under C:/Users/Public and has that
 * worker-shaped process launch cmd.exe with harmless discovery commands.
 */

#include "Common.hpp"

int wmain() {
  const auto worker = vigil::examples::public_examples_dir() / L"w3wp.exe";
  vigil::examples::copy_self_to(worker);

  if (vigil::examples::self_path().filename() == L"w3wp.exe") {
    std::wcout << L"vigil server_spawned_shell web-worker simulation\n";
    return vigil::examples::run_process(vigil::examples::system32(L"cmd.exe"),
                                        L"/c whoami && hostname");
  }

  return vigil::examples::run_process(worker);
}
