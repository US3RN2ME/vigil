#ifndef VIGIL_EXAMPLES_LINUX_COMMON_HPP
#define VIGIL_EXAMPLES_LINUX_COMMON_HPP

#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace vigil::examples {

   inline void sleep_for_agent(int seconds = 2) {
      std::this_thread::sleep_for(std::chrono::seconds(seconds));
   }

   inline std::filesystem::path self_path() {
      std::vector<char> buffer(4096);
      const auto size = ::readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
      if (size < 0) {
         perror("readlink(/proc/self/exe)");
         std::exit(1);
      }
      buffer[static_cast<std::size_t>(size)] = '\0';
      return buffer.data();
   }

   inline void copy_self_to(const std::filesystem::path& target) {
      std::filesystem::create_directories(target.parent_path());
      std::filesystem::copy_file(self_path(), target, std::filesystem::copy_options::overwrite_existing);
      std::filesystem::permissions(
          target, std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec,
          std::filesystem::perm_options::add);
   }

   [[noreturn]] inline void exec_path(const std::filesystem::path& path, std::vector<std::string> args) {
      std::vector<char*> argv;
      argv.reserve(args.size() + 2);
      argv.push_back(const_cast<char*>(path.c_str()));
      for (auto& arg : args)
         argv.push_back(arg.data());
      argv.push_back(nullptr);

      ::execv(path.c_str(), argv.data());
      perror("execv");
      std::exit(1);
   }

   inline int wait_for(pid_t pid) {
      int status = 0;
      while (::waitpid(pid, &status, 0) < 0) {
         if (errno != EINTR) {
            perror("waitpid");
            return 1;
         }
      }
      return status;
   }

   inline bool has_arg(int argc, char** argv, std::string_view expected) {
      for (int i = 1; i < argc; ++i) {
         if (argv[i] == expected)
            return true;
      }
      return false;
   }

   inline bool dangerous_allowed() {
      const char* value = std::getenv("VIGIL_ALLOW_DANGEROUS");
      return value != nullptr && std::string_view{value} == "1";
   }

   inline void require_dangerous() {
      if (!dangerous_allowed()) {
         std::cerr << "Refusing to run this invasive example without "
                      "VIGIL_ALLOW_DANGEROUS=1.\n";
         std::exit(2);
      }
   }

} // namespace vigil::examples

#endif // VIGIL_EXAMPLES_LINUX_COMMON_HPP
