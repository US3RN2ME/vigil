#include <algorithm>
#include <iterator>
#include <string_view>
#include <utility>
#include <vector>

#include <vigil/AlertDeduplicator.hpp>

namespace vigil {
   namespace {
      void appendField(std::string& key, const std::string_view value) {
         key += std::to_string(value.size());
         key += ':';
         key.append(value);
         key += ';';
      }
   } // namespace

   AlertDeduplicator::AlertDeduplicator(const std::chrono::milliseconds cooldown, const std::size_t maxEntries)
       : cooldown_{cooldown}
       , maxEntries_{std::max(maxEntries, std::size_t{1})} {}

   bool AlertDeduplicator::shouldEmit(const Alert& alert, const Clock::time_point now) {
      auto key = makeKey(alert);
      std::lock_guard lock{mutex_};

      if (const auto entry = entries_.find(key); entry != entries_.end()) {
         if (now - entry->second.lastEmitted < cooldown_)
            return false;

         lru_.erase(entry->second.lru);
         entries_.erase(entry);
      }

      while (entries_.size() >= maxEntries_) {
         entries_.erase(lru_.front());
         lru_.pop_front();
      }

      lru_.emplace_back(key);
      entries_.emplace(std::move(key), Entry{now, std::prev(lru_.end())});
      return true;
   }

   std::string AlertDeduplicator::makeKey(const Alert& alert) {
      std::string key;
      appendField(key, alert.rule);
      appendField(key, alert.severity);
      appendField(key, std::to_string(alert.info.pid));
      appendField(key, std::to_string(alert.info.startTimeNs));

      std::vector<std::pair<std::string_view, std::string_view>> attributes;
      attributes.reserve(alert.attributes.size());
      for (const auto& [name, value] : alert.attributes)
         attributes.emplace_back(name, value);

      std::ranges::sort(attributes);
      for (const auto& [name, value] : attributes) {
         appendField(key, name);
         appendField(key, value);
      }

      return key;
   }
} // namespace vigil
