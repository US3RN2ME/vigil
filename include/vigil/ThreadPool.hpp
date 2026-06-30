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
      using Task = std::function<void()>;

      explicit ThreadPool(std::size_t workerCount = defaultWorkerCount(), std::size_t maxQueuedTasks = 4096);
      ~ThreadPool();

      ThreadPool(const ThreadPool&) = delete;
      ThreadPool& operator=(const ThreadPool&) = delete;

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

      [[nodiscard]] std::size_t workerCount() const noexcept;
      [[nodiscard]] static std::size_t defaultWorkerCount() noexcept;

   private:
      bool submitToQueue(std::unique_lock<std::mutex>& lock, std::size_t queueIndex, Task task);
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
