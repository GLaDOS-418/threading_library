#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <iterator>
#include <thread>
#include <vector>

#include "concurrent_block_queue.h"

namespace utility {

  class ThreadJoiner {
    std::vector<std::thread> &joinable_threads;

    public:
    ThreadJoiner(std::vector<std::thread> &threads) : joinable_threads{threads} {}

    ~ThreadJoiner() {
      std::for_each(std::begin(joinable_threads), std::end(joinable_threads),
          [](auto &_thread) {
          if (_thread.joinable()) {
          _thread.join();
          }
          });
    }
  };

  class thread_pool {
    using WorkerPool = std::vector<std::thread>;

    ds::ConcurrentBlockQueue<std::function<void()>> tasks;
    WorkerPool workers;
    ThreadJoiner joiner;
    std::atomic_bool done;

    void worker_method() {
      while (!done) {
        auto work = tasks.wait_and_pop();
        work();
      }
    }

    public:
    thread_pool(const size_t total_threads = std::thread::hardware_concurrency())
      : joiner(workers), done(false)
    {
        //WorkerPool(total_threads, std::thread(&utility::thread_pool::worker_method,this)).swap(workers);
    }
  };
}; // namespace utility

#endif // THREAD_POOL_H
