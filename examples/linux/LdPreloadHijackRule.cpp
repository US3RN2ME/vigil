/**
 * @file LdPreloadHijackRule.cpp
 * @brief Simulates LD_PRELOAD hijacking with a benign preload shim.
 *
 * The program relaunches itself with LD_PRELOAD pointing to the companion
 * libvigil_preload_probe shared object. The preload constructor only writes a
 * marker under /tmp, but the process telemetry matches a preload hijack.
 */

#include <filesystem>

#include "Common.hpp"

int main(int argc, char** argv) {
   if (vigil::examples::has_arg(argc, argv, "--child")) {
      std::cout << "vigil ld_preload_hijack_rule benign preload simulation\n";
      vigil::examples::sleep_for_agent();
      return 0;
   }

   const auto preload = vigil::examples::self_path().parent_path().append("libvigil_preload_probe.so");
   if (!std::filesystem::exists(preload)) {
      std::cerr << "Missing " << preload << "\n"
                << "Build with CMake so libvigil_preload_probe.so is placed "
                   "next to this executable.\n";
      return 1;
   }

   // Benign analogue of a preload hijack: a library constructor runs before the
   // target process main(), but it only writes a marker file under /tmp.
   ::setenv("LD_PRELOAD", preload.c_str(), 1);
   vigil::examples::exec_path(vigil::examples::self_path(), {"--child"});
}
