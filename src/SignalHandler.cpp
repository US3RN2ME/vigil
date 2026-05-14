#include <mutex>

#include <vigil/SignalHandler.hpp>

namespace vigil {
   SignalHandler::SignalHandler() {
      install();
   }

   SignalHandler::~SignalHandler() {
      uninstall();
   }

   void SignalHandler::wait() {
      std::unique_lock lock(mutex_);

      cv_.wait(lock, [this] {
         return stopRequested_;
      });
   }

   void SignalHandler::requestStop(Reason reason) {
      {
         std::lock_guard lock(mutex_);

         if (stopRequested_) {
            return;
         }

         stopRequested_ = true;
         reason_ = reason;
      }
      onStopRequested.emit(reason);
      cv_.notify_all();
   }

   bool SignalHandler::stopRequested() const {
      std::lock_guard lock(mutex_);
      return stopRequested_;
   }

   SignalHandler::Reason SignalHandler::reason() const {
      std::lock_guard lock(mutex_);
      return reason_;
   }
} // namespace vigil
