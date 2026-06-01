
#ifndef VIGIL_SIGNAL_HPP
#define VIGIL_SIGNAL_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace vigil {
   /**
    * @brief Lightweight in-process signal/slot helper.
    *
    * Callback registration is internally synchronized. Slots are invoked
    * without holding the signal lock, so callbacks
    * may safely connect or
    * disconnect slots while an emission is in progress.
    *
    * @tparam Ts
    * Argument types delivered to connected slots.
    */
   template <class... Ts>
   class Signal {
   public:
      /**
       * @brief Callback type invoked when the signal is emitted.
       */
      using Slot = std::function<void(Ts...)>;

      /**
       * @brief Register a callback and return its connection id.
       *
       * @param slot Callback to invoke
       * when the signal is emitted.
       *
       * @return Connection id that can be passed to disconnect().
       */
      uint32_t connect(Slot slot) {
         std::lock_guard lock(mutex_);
         auto id = nextId_++;
         handlers_.emplace(id, std::move(slot));
         return id;
      }

      /**
       * @brief Disconnect a previously registered callback.
       *
       * @param id Connection id returned by
       * connect().
       *
       * @return true if a callback was removed.
       */
      bool disconnect(uint32_t id) {
         std::lock_guard lock(mutex_);
         return handlers_.erase(id) > 0;
      }

      /**
       * @brief Invoke all connected callbacks with the supplied arguments.
       *
       * @param args Arguments
       * forwarded to each connected slot.
       */
      void emit(const Ts&... args) {
         std::vector<Slot> slots;
         {
            std::lock_guard lock(mutex_);
            slots.reserve(handlers_.size());
            for (const auto& [id, slot] : handlers_) {
               (void)id;
               slots.emplace_back(slot);
            }
         }

         for (const auto& slot : slots) {
            slot(args...);
         }
      }

   private:
      std::mutex mutex_;
      std::unordered_map<uint32_t, Slot> handlers_;
      uint32_t nextId_{1};
   };
} // namespace vigil

#endif // VIGIL_SIGNAL_HPP
