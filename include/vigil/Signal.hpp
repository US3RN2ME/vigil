
#ifndef VIGIL_SIGNAL_HPP
#define VIGIL_SIGNAL_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace vigil {
   /**
    * @brief Lightweight in-process signal/slot helper.
    *
    * Signal is not internally synchronized. Callers should
    * connect, disconnect,
    * and emit from a single thread or provide external synchronization.
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
         return handlers_.erase(id) > 0;
      }

      /**
       * @brief Invoke all connected callbacks with the supplied arguments.
       *
       * @param args Arguments
       * forwarded to each connected slot.
       */
      void emit(const Ts&... args) {
         for (const auto& [id, slot] : handlers_) {
            (void)id;
            slot(args...);
         }
      }

   private:
      std::unordered_map<uint32_t, Slot> handlers_;
      uint32_t nextId_{1};
   };
} // namespace vigil

#endif // VIGIL_SIGNAL_HPP
