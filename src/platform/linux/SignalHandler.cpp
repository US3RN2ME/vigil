
#include <atomic>
#include <csignal>
#include <mutex>
#include <pthread.h>
#include <stdexcept>
#include <thread>

#include <vigil/Error.hpp>
#include <vigil/SignalHandler.hpp>

namespace vigil {
   namespace {

      std::mutex activeHandlerMutex;
      SignalHandler* activeHandler = nullptr;

      sigset_t g_signals{};
      std::thread g_thread;
      std::atomic_bool g_stopping{false};

      SignalHandler::Reason toReason(int signal) {
         switch (signal) {
            case SIGINT:
               return SignalHandler::Reason::Interrupt;
            case SIGTERM:
               return SignalHandler::Reason::Terminate;
            case SIGQUIT:
               return SignalHandler::Reason::Quit;
            case SIGHUP:
               return SignalHandler::Reason::Hangup;
            default:
               return SignalHandler::Reason::Terminate;
         }
      }

      SignalHandler* currentHandler() {
         std::lock_guard lock(activeHandlerMutex);
         return activeHandler;
      }

   } // namespace

   void SignalHandler::install() {
      {
         std::lock_guard lock(activeHandlerMutex);

         if (activeHandler != nullptr) {
            throw SignalHandlerError{"Only one ShutdownSignal may be active per process"};
         }

         activeHandler = this;
      }

      g_stopping = false;

      sigemptyset(&g_signals);

      sigaddset(&g_signals, SIGINT);
      sigaddset(&g_signals, SIGTERM);
      sigaddset(&g_signals, SIGQUIT);
      sigaddset(&g_signals, SIGHUP);

      // Internal signal used to unblock sigwait() in destructor.
      sigaddset(&g_signals, SIGUSR1);

      if (pthread_sigmask(SIG_BLOCK, &g_signals, nullptr) != 0) {
         std::lock_guard lock(activeHandlerMutex);
         activeHandler = nullptr;

         throw SignalHandlerError{"Failed to block shutdown signals"};
      }

      g_thread = std::thread([] {
         while (true) {
            int signal = 0;

            if (sigwait(&g_signals, &signal) != 0) {
               continue;
            }

            if (signal == SIGUSR1 && g_stopping.load()) {
               break;
            }

            if (auto* handler = currentHandler()) {
               handler->requestStop(toReason(signal));
            }

            break;
         }
      });
   }

   void SignalHandler::uninstall() {
      g_stopping = true;

      if (g_thread.joinable()) {
         pthread_kill(g_thread.native_handle(), SIGUSR1);
         g_thread.join();
      }

      std::lock_guard lock(activeHandlerMutex);

      if (activeHandler == this) {
         activeHandler = nullptr;
      }
   }
} // namespace vigil
