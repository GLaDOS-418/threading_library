#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <iterator>
#include <thread>
#include <vector>

#include <ds/concurrent_block_queue.h>

namespace utility {

  class thread_pool {
    using TaskT = std::function<void( )>;
    using TaskGroupT = ds::ConcurrentBlockQueue<TaskT>;
    using WorkerGroupT = std::vector<std::jthread>;

    TaskGroupT tasks;
    WorkerGroupT workers;
    std::atomic_bool done{false}; // true = complete all remaining work & exit

    void worker_method() {
      while (!done) {
        auto work = tasks.wait_and_pop();
        work();
      }
    }

    public:
    thread_pool(const size_t total_workers = std::thread::hardware_concurrency()) : done(false)
    { 
      for(size_t i = 0; i < total_workers; ++i) {
        workers.emplace_back(std::jthread(&utility::thread_pool::worker_method,this));
      }
    }
  };
}; // namespace utility

#endif // THREAD_POOL_H
