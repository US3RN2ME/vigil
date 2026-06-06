#include <atomic>
#include <csignal>
#include <mutex>
#include <pthread.h>

#include <vigil/Error.hpp>
#include <vigil/SignalHandler.hpp>

namespace vigil {
namespace {

constexpr int kWakeSignal = SIGUSR1;

std::atomic_bool active{false};
std::mutex platformMutex;
sigset_t signals{};
sigset_t previousMask{};
pthread_t waiterThread{};
bool waiterRegistered{false};
bool maskInstalled{false};

StopReason toStopReason(int signal) {
  switch (signal) {
  case SIGINT:
    return StopReason::Interrupt;
  case SIGTERM:
    return StopReason::Terminate;
  case SIGQUIT:
    return StopReason::Quit;
  case SIGHUP:
    return StopReason::Hangup;
  default:
    return StopReason::Terminate;
  }
}

} // namespace

void SignalHandler::wait() {
  {
    std::lock_guard lock(mutex_);

    if (stopRequested_) {
      return;
    }
  }

  {
    std::lock_guard lock(platformMutex);
    waiterThread = pthread_self();
    waiterRegistered = true;
  }

  while (!stopRequested()) {
    int signal = 0;
    if (sigwait(&signals, &signal) != 0) {
      continue;
    }

    if (signal == kWakeSignal) {
      continue;
    }

    requestStop(toStopReason(signal));
  }

  std::lock_guard lock(platformMutex);
  waiterRegistered = false;
}

void SignalHandler::install() {
  bool expected = false;
  if (!active.compare_exchange_strong(expected, true,
                                      std::memory_order_acq_rel)) {
    throw SignalHandlerError{
        "Only one SignalHandler may be active per process"};
  }

  sigemptyset(&signals);

  sigaddset(&signals, SIGINT);
  sigaddset(&signals, SIGTERM);
  sigaddset(&signals, SIGQUIT);
  sigaddset(&signals, SIGHUP);
  sigaddset(&signals, kWakeSignal);

  if (pthread_sigmask(SIG_BLOCK, &signals, &previousMask) != 0) {
    active.store(false, std::memory_order_release);
    throw SignalHandlerError{"Failed to block shutdown signals"};
  }

  maskInstalled = true;
}

void SignalHandler::uninstall() {
  {
    std::lock_guard lock(platformMutex);

    if (waiterRegistered && !pthread_equal(waiterThread, pthread_self())) {
      pthread_kill(waiterThread, kWakeSignal);
    }

    waiterRegistered = false;
    active.store(false, std::memory_order_release);
  }

  if (maskInstalled) {
    pthread_sigmask(SIG_SETMASK, &previousMask, nullptr);
    maskInstalled = false;
  }
}

void SignalHandler::notifyStopRequested() {
  std::lock_guard lock(platformMutex);

  if (!waiterRegistered || pthread_equal(waiterThread, pthread_self())) {
    return;
  }

  pthread_kill(waiterThread, kWakeSignal);
}
} // namespace vigil
