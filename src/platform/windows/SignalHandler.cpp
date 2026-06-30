#include <atomic>
#include <condition_variable>

#include "WinApi.hpp"

#include <vigil/Error.hpp>
#include <vigil/SignalHandler.hpp>

namespace vigil {
   namespace {
      std::atomic<SignalHandler*> activeHandler{nullptr};
      std::condition_variable stopRequestedCv;

      StopReason toStopReason(DWORD event) {
         switch (event) {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
               return StopReason::Interrupt;
            case CTRL_CLOSE_EVENT:
               return StopReason::ConsoleClose;
            case CTRL_LOGOFF_EVENT:
               return StopReason::Logoff;
            case CTRL_SHUTDOWN_EVENT:
               return StopReason::Shutdown;
            default:
               return StopReason::Terminate;
         }
      }

      BOOL WINAPI consoleHandler(DWORD event) {
         auto* handler = activeHandler.load(std::memory_order_acquire);
         if (handler == nullptr) {
            return FALSE;
         }

         handler->requestStop(toStopReason(event));
         return TRUE;
      }

   } // namespace

   void SignalHandler::wait() {
      std::unique_lock lock(mutex_);

      stopRequestedCv.wait(lock, [this] {
         return stopRequested_;
      });
   }

   void SignalHandler::install() {
      SignalHandler* expected = nullptr;
      if (!activeHandler.compare_exchange_strong(expected, this, std::memory_order_acq_rel)) {
         throw SignalHandlerError{"Only one SignalHandler may be active per process"};
      }

      if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
         activeHandler.store(nullptr, std::memory_order_release);
         throw SignalHandlerError{"Failed to install console control handler"};
      }
   }

   void SignalHandler::uninstall() {
      SignalHandler* expected = this;
      if (!activeHandler.compare_exchange_strong(expected, nullptr, std::memory_order_acq_rel)) {
         return;
      }

      SetConsoleCtrlHandler(consoleHandler, FALSE);
   }

   void SignalHandler::notifyStopRequested() {
      stopRequestedCv.notify_all();
   }
} // namespace vigil
