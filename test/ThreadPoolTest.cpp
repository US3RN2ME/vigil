#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>

#include "ut_main.hpp"

#include <vigil/ThreadPool.hpp>

namespace {
   suite<"[ThreadPool]"> _ = [] {
      "[ExecutesTasksConcurrently]"_test = [] {
         vigil::ThreadPool pool{4};
         std::atomic<int> active = 0;
         std::atomic<int> maxActive = 0;

         for (int i = 0; i < 8; ++i) {
            pool.submit([&] {
               const int current = ++active;
               int observed = maxActive;
               while ((current > observed) && !maxActive.compare_exchange_weak(observed, current)) {
               }

               std::this_thread::sleep_for(std::chrono::milliseconds{5});
               --active;
            });
         }

         pool.waitIdle();
         expect(maxActive.load() > 1);
      };

      "[StopDrainsQueuedTasksAndRejectsNewWork]"_test = [] {
         vigil::ThreadPool pool{2};
         std::atomic<int> completed = 0;

         for (int i = 0; i < 10; ++i) {
            pool.submit([&] {
               ++completed;
            });
         }

         pool.stop();

         expect(eq(completed.load(), 10));
         expect(!pool.submit([] {}));
      };

      "[WorkerSurvivesTaskException]"_test = [] {
         vigil::ThreadPool pool{1};
         std::atomic<bool> completed = false;

         pool.submit([] {
            throw std::runtime_error{"boom"};
         });
         pool.submit([&] {
            completed = true;
         });

         pool.waitIdle();
         expect(completed.load());
      };

      "[AffinityTasksExecuteSerially]"_test = [] {
         vigil::ThreadPool pool{4};
         std::atomic<int> active = 0;
         std::atomic<int> maxActive = 0;

         for (int i = 0; i < 8; ++i) {
            pool.submit(42, [&] {
               const int current = ++active;
               int observed = maxActive;
               while ((current > observed) && !maxActive.compare_exchange_weak(observed, current)) {
               }

               std::this_thread::sleep_for(std::chrono::milliseconds{1});
               --active;
            });
         }

         pool.waitIdle();
         expect(eq(maxActive.load(), 1));
      };

      "[StopUnblocksWaitingSubmission]"_test = [] {
         vigil::ThreadPool pool{1, 1};
         std::atomic<bool> releaseWorker = false;
         std::atomic<bool> submitReturned = false;

         pool.submit([&] {
            while (!releaseWorker)
               std::this_thread::yield();
         });
         pool.submit([] {});

         std::thread submitter([&] {
            submitReturned = !pool.submit([] {});
         });

         std::thread stopper([&] {
            pool.stop();
         });

         while (!submitReturned)
            std::this_thread::yield();
         releaseWorker = true;
         stopper.join();
         submitter.join();

         expect(submitReturned.load());
      };
   };
} // namespace
