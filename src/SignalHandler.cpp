
#include <cstdio>
#include <mutex>

#include <vigil/SignalHandler.hpp>

namespace vigil {
   std::string_view toStringView(StopReason reason) {
      switch (reason) {
         case StopReason::Interrupt:
            return "interrupt";
         case StopReason::Terminate:
            return "terminate";
         case StopReason::Quit:
            return "quit";
         case StopReason::Hangup:
            return "hangup";
         case StopReason::ConsoleClose:
            return "console close";
         case StopReason::Logoff:
            return "logoff";
         case StopReason::Shutdown:
            return "shutdown";
         case StopReason::Programmatic:
            return "programmatic";
         case StopReason::None:
            return "none";
      }
      return "unknown";
   }

   void waitForExitAcknowledgement(std::string_view prompt) {
      std::fwrite(prompt.data(), sizeof(char), prompt.size(), stdout);
      std::fflush(stdout);
      std::getchar();
   }

   SignalHandler::SignalHandler() {
      install();
   }

   SignalHandler::~SignalHandler() {
      uninstall();
   }

   void SignalHandler::requestStop(StopReason reason) {
      {
         std::lock_guard lock(mutex_);

         if (stopRequested_) {
            return;
         }

         stopRequested_ = true;
         reason_ = reason;
      }

      notifyStopRequested();
   }

   bool SignalHandler::stopRequested() const {
      std::lock_guard lock(mutex_);
      return stopRequested_;
   }

   StopReason SignalHandler::reason() const {
      std::lock_guard lock(mutex_);
      return reason_;
   }

} // namespace vigil
