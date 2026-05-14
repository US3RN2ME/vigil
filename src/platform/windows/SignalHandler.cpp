
#include "WinApi.hpp"

#include <vigil/Error.hpp>
#include <vigil/SignalHandler.hpp>

namespace vigil {
   namespace {

      std::mutex activeHandlerMutex;
      SignalHandler* activeHandler = nullptr;

      SignalHandler::Reason toReason(DWORD event) {
         switch (event) {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
               return SignalHandler::Reason::Interrupt;

            case CTRL_CLOSE_EVENT:
               return SignalHandler::Reason::ConsoleClose;

            case CTRL_LOGOFF_EVENT:
               return SignalHandler::Reason::Logoff;

            case CTRL_SHUTDOWN_EVENT:
               return SignalHandler::Reason::Shutdown;

            default:
               return SignalHandler::Reason::Terminate;
         }
      }

      SignalHandler* currentHandler() {
         std::lock_guard lock(activeHandlerMutex);
         return activeHandler;
      }

      BOOL WINAPI consoleHandler(DWORD event) {
         if (auto* handler = currentHandler()) {
            handler->requestStop(toReason(event));
            return true;
         }

         return false;
      }

   } // namespace

   void SignalHandler::install() {
      std::lock_guard lock(activeHandlerMutex);

      if (activeHandler != nullptr) {
         throw SignalHandlerError{"Only one ShutdownSignal may be active per process"};
      }

      activeHandler = this;

      if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
         activeHandler = nullptr;
         throw SignalHandlerError{"Failed to install console control handler"};
      }
   }

   void SignalHandler::uninstall() {
      std::lock_guard lock(activeHandlerMutex);

      if (activeHandler == this) {
         SetConsoleCtrlHandler(consoleHandler, FALSE);
         activeHandler = nullptr;
      }
   }
} // namespace vigil
