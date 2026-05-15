
#ifndef VIGIL_SIGNALHANDLER_HPP
#define VIGIL_SIGNALHANDLER_HPP

#include <mutex>
#include <string_view>

namespace vigil {

   enum class StopReason { Interrupt, Terminate, Quit, Hangup, ConsoleClose, Logoff, Shutdown, Programmatic, None };

   std::string_view toStringView(StopReason reason);

   void waitForExitAcknowledgement(std::string_view prompt = "Press enter to exit...");

   class SignalHandler {
   public:
      SignalHandler();
      ~SignalHandler();

      SignalHandler(const SignalHandler&) = delete;
      SignalHandler& operator=(const SignalHandler&) = delete;

      void wait();
      void requestStop(StopReason reason = StopReason::Programmatic);

      [[nodiscard]] bool stopRequested() const;
      [[nodiscard]] StopReason reason() const;

   private:
      void install();
      void uninstall();
      void notifyStopRequested();

      mutable std::mutex mutex_;
      bool stopRequested_{false};
      StopReason reason_{StopReason::None};
   };
} // namespace vigil

#endif // VIGIL_SIGNALHANDLER_HPP
