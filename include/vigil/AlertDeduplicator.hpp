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
/**
 * @brief Suppresses repeated alerts for the same process and rule key.
 *
 * The deduplicator is thread-safe and keeps a bounded least-recently-used
 * cache so long-running agents do not accumulate unlimited alert keys.
 */
class AlertDeduplicator {
public:
  /**
   * @brief Monotonic clock used for cooldown comparisons.
   */
  using Clock = std::chrono::steady_clock;

  /**
   * @brief Construct an alert deduplicator.
   *
   * @param cooldown Minimum time between equivalent alerts.
   * @param maxEntries Maximum number of alert keys retained in the cache.
   */
  explicit AlertDeduplicator(
      std::chrono::milliseconds cooldown = std::chrono::minutes{1},
      std::size_t maxEntries = 4096);

  /**
   * @brief Return whether an alert should be emitted now.
   *
   * Equivalent alerts inside the cooldown window are suppressed. Alerts outside
   * the window update the cache and are allowed through.
   *
   * @param alert Alert candidate.
   * @param now Time used for the cooldown comparison.
   *
   * @return true when the caller should emit the alert.
   */
  [[nodiscard]] bool shouldEmit(const Alert &alert,
                                Clock::time_point now = Clock::now());

private:
  struct Entry {
    Clock::time_point lastEmitted;
    std::list<std::string>::iterator lru;
  };

  [[nodiscard]] static std::string makeKey(const Alert &alert);

  std::chrono::milliseconds cooldown_;
  std::size_t maxEntries_;
  std::mutex mutex_;
  std::list<std::string> lru_;
  std::unordered_map<std::string, Entry> entries_;
};
} // namespace vigil

#endif
