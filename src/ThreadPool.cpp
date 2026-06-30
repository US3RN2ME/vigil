#include <algorithm>
#include <exception>

#include <vigil/Logger.hpp>
#include <vigil/ThreadPool.hpp>

namespace vigil {
   ThreadPool::ThreadPool(std::size_t workerCount, std::size_t maxQueuedTasks)
       : maxQueuedTasks_{std::max<std::size_t>(maxQueuedTasks, 1)} {
      const auto count = std::max<std::size_t>(workerCount, 1);
      taskQueues_.resize(count);
      workers_.reserve(count);
      for (std::size_t i = 0; i < count; ++i) {
         workers_.emplace_back([this, i](std::stop_token stopToken) {
            run(stopToken, i);
         });
      }
   }

   ThreadPool::~ThreadPool() {
      stop();
   }

   bool ThreadPool::submit(Task task) {
      std::unique_lock lock(mutex_);
      const auto queueIndex = nextQueue_++ % taskQueues_.size();
      return submitToQueue(lock, queueIndex, std::move(task));
   }

   bool ThreadPool::submit(std::size_t affinityKey, Task task) {
      std::unique_lock lock(mutex_);
      return submitToQueue(lock, affinityKey % taskQueues_.size(), std::move(task));
   }

   bool ThreadPool::submitToQueue(std::unique_lock<std::mutex>& lock, std::size_t queueIndex, Task task) {
      queueCapacityAvailable_.wait(lock, [this] {
         return !acceptingTasks_ || queuedTasks_ < maxQueuedTasks_;
      });

      if (!acceptingTasks_)
         return false;

      taskQueues_[queueIndex].emplace(std::move(task));
      ++queuedTasks_;
      lock.unlock();
      taskAvailable_.notify_all();
      return true;
   }

   bool ThreadPool::queuesEmpty() const {
      return std::ranges::all_of(taskQueues_, [](const auto& queue) {
         return queue.empty();
      });
   }

   void ThreadPool::waitIdle() {
      std::unique_lock lock(mutex_);
      idle_.wait(lock, [this] {
         return queuesEmpty() && activeTasks_ == 0;
      });
   }

   void ThreadPool::stop() {
      {
         std::lock_guard lock(mutex_);
         if (!acceptingTasks_ && workers_.empty())
            return;
         acceptingTasks_ = false;
      }

      for (auto& worker : workers_)
         worker.request_stop();
      taskAvailable_.notify_all();
      queueCapacityAvailable_.notify_all();
      workers_.clear();
   }

   std::size_t ThreadPool::workerCount() const noexcept {
      std::lock_guard lock(mutex_);
      return workers_.size();
   }

   std::size_t ThreadPool::defaultWorkerCount() noexcept {
      constexpr std::size_t kMaxWorkers = 4;
      return std::clamp<std::size_t>(std::thread::hardware_concurrency(), 1, kMaxWorkers);
   }

   void ThreadPool::run(std::stop_token stopToken, std::size_t queueIndex) {
      while (true) {
         Task task;
         {
            std::unique_lock lock(mutex_);
            taskAvailable_.wait(lock, stopToken, [this, queueIndex] {
               return !taskQueues_[queueIndex].empty();
            });

            auto& tasks = taskQueues_[queueIndex];
            if (tasks.empty()) {
               if (stopToken.stop_requested())
                  return;
               continue;
            }

            task = std::move(tasks.front());
            tasks.pop();
            --queuedTasks_;
            ++activeTasks_;
         }

         queueCapacityAvailable_.notify_one();

         try {
            task();
         } catch (const std::exception& e) {
            log::error("thread pool task failed: {}", e.what());
         } catch (...) {
            log::error("thread pool task failed with unknown error");
         }

         bool idle = false;
         {
            std::lock_guard lock(mutex_);
            --activeTasks_;
            if (queuesEmpty() && activeTasks_ == 0)
               idle = true;
         }

         if (idle)
            idle_.notify_all();
      }
   }
} // namespace vigil
