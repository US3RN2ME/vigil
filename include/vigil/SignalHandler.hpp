
#ifndef VIGIL_SIGNALHANDLER_HPP
#define VIGIL_SIGNALHANDLER_HPP

#include <mutex>
#include <thread>

#include <vigil/Signal.hpp>

namespace vigil {

   class SignalHandler {
   public:
      enum class Reason { Interrupt, Terminate, Quit, Hangup, ConsoleClose, Logoff, Shutdown, Programmatic };

      Signal<Reason> onStopRequested;

      SignalHandler();
      ~SignalHandler();

      SignalHandler(const SignalHandler&) = delete;
      SignalHandler& operator=(const SignalHandler&) = delete;

      void wait();
      void requestStop(Reason reason = Reason::Programmatic);

      [[nodiscard]] bool stopRequested() const;
      [[nodiscard]] Reason reason() const;

   private:
      void install();
      void uninstall();

      mutable std::mutex mutex_;
      std::condition_variable cv_;

      bool stopRequested_{false};
      Reason reason_{Reason::Programmatic};
   };
} // namespace vigil

#endif // VIGIL_SIGNALHANDLER_HPP
