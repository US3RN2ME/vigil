
#ifndef VIGIL_SIGNAL_HPP
#define VIGIL_SIGNAL_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace vigil {
   template <class... Ts>
   class Signal {
  public:
      using Slot = std::function<void(Ts...)>;

      uint32_t connect(Slot slot) {
         auto id = nextId_++;
         handlers_.emplace(id, std::move(slot));
         return id;
      }

      bool disconnect(uint32_t id) {
         return handlers_.erase(id) > 0;
      }

      void emit(const Ts&... args) {
         for (const auto& [_, slot] : handlers_) {
            slot(args...);
         }
      }

  private:
      std::unordered_map<uint32_t, Slot> handlers_;
      uint32_t nextId_{1};
   };
} // namespace vigil

#endif // VIGIL_SIGNAL_HPP
