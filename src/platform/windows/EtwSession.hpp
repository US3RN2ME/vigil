
#ifndef VIGIL_PLATFORM_ETWSESSION_HPP
#define VIGIL_PLATFORM_ETWSESSION_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <string>

struct _EVENT_RECORD;

namespace vigil::platform {

   class EtwSession {
      static constexpr auto kInvalid = static_cast<uint64_t>(-1);

   public:
      using EventCallback = std::function<void(const _EVENT_RECORD&)>;

      static std::optional<EtwSession> start(std::wstring sessionName, std::array<uint8_t, 16> providerGuid,
                                             uint64_t keyword = 0xFFFFFFFFFFFFFFFFULL);
      ~EtwSession();

      EtwSession(const EtwSession&) = delete;
      EtwSession& operator=(const EtwSession&) = delete;
      EtwSession(EtwSession&&) noexcept;
      EtwSession& operator=(EtwSession&&) noexcept;

      void notify(const _EVENT_RECORD& event);

      void consume(EventCallback callback);
      void stop();

   private:
      EtwSession() = default;

      std::wstring name_;
      std::array<uint8_t, 16> providerGuid_{};
      uint64_t sessionHandle_{kInvalid};
      uint64_t consumerHandle_{kInvalid};
      EventCallback callback_;
      std::mutex mutex_;
      bool stopping_{false};
   };

} // namespace vigil::platform

#endif // VIGIL_PLATFORM_ETWSESSION_HPP
