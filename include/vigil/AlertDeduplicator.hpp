#ifndef VIGIL_ALERTDEDUPLICATOR_HPP
#define VIGIL_ALERTDEDUPLICATOR_HPP

#include <chrono>
#include <cstddef>
#include <list>
#include <mutex>
#include <string>
#include <unordered_map>

#include <vigil/Alert.hpp>

namespace vigil {
   class AlertDeduplicator {
   public:
      using Clock = std::chrono::steady_clock;

      explicit AlertDeduplicator(std::chrono::milliseconds cooldown = std::chrono::minutes{1}, std::size_t maxEntries = 4096);

      [[nodiscard]] bool shouldEmit(const Alert& alert, Clock::time_point now = Clock::now());

   private:
      struct Entry {
         Clock::time_point lastEmitted;
         std::list<std::string>::iterator lru;
      };

      [[nodiscard]] static std::string makeKey(const Alert& alert);

      std::chrono::milliseconds cooldown_;
      std::size_t maxEntries_;
      std::mutex mutex_;
      std::list<std::string> lru_;
      std::unordered_map<std::string, Entry> entries_;
   };
} // namespace vigil

#endif
