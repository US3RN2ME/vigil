
#ifndef VIGIL_SIGNALHANDLER_HPP
#define VIGIL_SIGNALHANDLER_HPP

#include <mutex>
#include <string_view>

namespace vigil {

/**
 * @brief Reason the agent was asked to stop.
 */
enum class StopReason {
  Interrupt,
  Terminate,
  Quit,
  Hangup,
  ConsoleClose,
  Logoff,
  Shutdown,
  Programmatic,
  None
};

/**
 * @brief Convert a stop reason to a stable human-readable string.
 *
 * @param reason Stop reason to convert.
 *
 * @return Stable string representation of the stop reason.
 */
std::string_view toStringView(StopReason reason);

/**
 * @brief Prompt the user and block until an exit acknowledgement is received.
 *
 * @param prompt Text displayed before waiting for acknowledgement.
 */
void waitForExitAcknowledgement(
    std::string_view prompt = "Press enter to exit...");

/**
 * @brief Cross-platform process lifetime signal handler.
 */
class SignalHandler {
public:
  /**
   * @brief Install OS signal or console-control handlers.
   *
   * @throws SignalHandlerError if handler installation fails.
   */
  SignalHandler();

  /**
   * @brief Uninstall handlers owned by this instance.
   */
  ~SignalHandler();

  SignalHandler(const SignalHandler &) = delete;
  SignalHandler &operator=(const SignalHandler &) = delete;

  /**
   * @brief Block until a stop request is received.
   */
  void wait();

  /**
   * @brief Request shutdown programmatically.
   *
   * @param reason Stop reason to record.
   */
  void requestStop(StopReason reason = StopReason::Programmatic);

  /**
   * @brief Return true after an OS or programmatic stop request.
   *
   * @return true if shutdown has been requested.
   */
  [[nodiscard]] bool stopRequested() const;

  /**
   * @brief Return the reason associated with the current stop request.
   *
   * @return Recorded stop reason.
   */
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
