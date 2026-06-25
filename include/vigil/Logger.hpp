
#ifndef VIGIL_LOGGER_HPP
#define VIGIL_LOGGER_HPP

#include <format>
#include <string_view>
#include <utility>

namespace vigil::log {
/**
 * @brief RAII handle that shuts down logging when it leaves scope.
 *
 *
 * Instances are returned by init(). Moving transfers shutdown ownership to the

 * * destination guard.
 */
class ShutdownGuard {
public:
  /**
   * @brief Logging shutdown ownership cannot be copied.
   */
  ShutdownGuard(const ShutdownGuard &) = delete;

  /**
   * @brief Logging shutdown ownership cannot be copy-assigned.
   */
  ShutdownGuard &operator=(const ShutdownGuard &) = delete;

  /**
   * @brief Transfer logging shutdown ownership from another guard.
   *

   * * @param other Guard whose ownership is transferred.
   */
  ShutdownGuard(ShutdownGuard &&other) noexcept;

  /**
   * @brief Transfer logging shutdown ownership from another guard.
   *

   * * @param other Guard whose ownership is transferred.
   *
   * @return
   * Reference to this guard.
   */
  ShutdownGuard &operator=(ShutdownGuard &&other) noexcept;

  /**
   * @brief Shut down logging if this guard owns the active logger.
   */
  ~ShutdownGuard();

private:
  friend ShutdownGuard init(std::string_view logFile);

  ShutdownGuard() = default;

  bool active_{true};
};

/**
 * @brief Log a debug message.
 *
 * @param msg Message text.
 */
void debug(std::string_view msg);

/**
 * @brief Log an informational message.
 *
 * @param msg Message text.
 */
void info(std::string_view msg);

/**
 * @brief Log a warning message.
 *
 * @param msg Message text.
 */
void warn(std::string_view msg);

/**
 * @brief Log an error message.
 *
 * @param msg Message text.
 */
void error(std::string_view msg);

/**
 * @brief Format and log a debug message.
 *
 * @tparam Args Format argument types.
 *
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <class... Args>
void debug(std::format_string<Args...> fmt, Args &&...args) {
  debug(std::format(fmt, std::forward<Args>(args)...));
}

/**
 * @brief Format and log an informational message.
 *
 * @tparam Args Format argument types.
 *
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <class... Args>
void info(std::format_string<Args...> fmt, Args &&...args) {
  info(std::format(fmt, std::forward<Args>(args)...));
}

/**
 * @brief Format and log a warning message.
 *
 * @tparam Args Format argument types.
 *
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <class... Args>
void warn(std::format_string<Args...> fmt, Args &&...args) {
  warn(std::format(fmt, std::forward<Args>(args)...));
}

/**
 * @brief Format and log an error message.
 *
 * @tparam Args Format argument types.
 *
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <class... Args>
void error(std::format_string<Args...> fmt, Args &&...args) {
  error(std::format(fmt, std::forward<Args>(args)...));
}

/**
 * @brief Initialize logging. When logFile is empty, logs are written to stderr
 * only.
 *
 * @param logFile Optional file path for persistent log output.
 */
[[nodiscard]] ShutdownGuard init(std::string_view logFile = "");

/**
 * @brief Flush and release logging resources.
 */
void shutdown();
} // namespace vigil::log

#endif // VIGIL_LOGGER_HPP
