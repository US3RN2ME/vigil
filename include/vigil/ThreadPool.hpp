#ifndef VIGIL_THREADPOOL_HPP
#define VIGIL_THREADPOOL_HPP

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace vigil {
/**
 * @brief Small fixed-size worker pool with cooperative shutdown.
 *
 * Existing tasks are drained during stop(). New tasks are rejected after
 * shutdown begins.
 */
class ThreadPool {
public:
  /**
   * @brief Unit of work executed by a worker thread.
   */
  using Task = std::function<void()>;

  /**
   * @brief Create a fixed-size worker pool.
   *
   * @param workerCount Number of worker threads to start.
   * @param maxQueuedTasks Maximum total queued tasks before submit() blocks.
   */
  explicit ThreadPool(std::size_t workerCount = defaultWorkerCount(),
                      std::size_t maxQueuedTasks = 4096);

  /**
   * @brief Stop the pool and join all worker threads.
   */
  ~ThreadPool();

  /**
   * @brief Thread pools own worker threads and cannot be copied.
   */
  ThreadPool(const ThreadPool &) = delete;

  /**
   * @brief Thread pools own worker threads and cannot be copy-assigned.
   */
  ThreadPool &operator=(const ThreadPool &) = delete;

  /**
   * @brief Queue work for asynchronous execution.
   *
   * Blocks while the bounded queue is full.
   *
   * @return false if shutdown has already started.
   */
  bool submit(Task task);

  /**
   * @brief Queue work on a stable worker selected by an affinity key.
   *
   * Tasks submitted with the same key execute in FIFO order. Blocks while
   * the bounded queue is full.
   *
   * @return false if shutdown has already started.
   */
  bool submit(std::size_t affinityKey, Task task);

  /**
   * @brief Block until all queued and active tasks complete.
   */
  void waitIdle();

  /**
   * @brief Stop accepting work, drain queued tasks, and join workers.
   */
  void stop();

  /**
   * @brief Return the number of worker threads in this pool.
   */
  [[nodiscard]] std::size_t workerCount() const noexcept;

  /**
   * @brief Return the default worker count for this host.
   */
  [[nodiscard]] static std::size_t defaultWorkerCount() noexcept;

private:
  bool submitToQueue(std::unique_lock<std::mutex> &lock, std::size_t queueIndex,
                     Task task);
  [[nodiscard]] bool queuesEmpty() const;
  void run(std::stop_token stopToken, std::size_t queueIndex);

  mutable std::mutex mutex_;
  std::condition_variable_any taskAvailable_;
  std::condition_variable queueCapacityAvailable_;
  std::condition_variable idle_;
  std::vector<std::queue<Task>> taskQueues_;
  std::vector<std::jthread> workers_;
  std::size_t activeTasks_{0};
  std::size_t queuedTasks_{0};
  std::size_t nextQueue_{0};
  std::size_t maxQueuedTasks_;
  bool acceptingTasks_{true};
};
} // namespace vigil

#endif // VIGIL_THREADPOOL_HPP
