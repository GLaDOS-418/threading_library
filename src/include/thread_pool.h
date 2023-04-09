#include <cstddef>
#include <iterator>
#include <thread>
#include <vector>
#include <functional>
#include <algorithm>
#include <atomic>

#include "multithreaded_queue.h"

class ThreadJoiner {
  std::vector<std::thread>& joinable_threads;

  public:
  ThreadJoiner(std::vector<std::thread>& threads) : joinable_threads{threads}
  {}
  
  ~ThreadJoiner() {
    std::for_each(std::begin(joinable_threads),std::end(joinable_threads),[](auto& _thread) {
          if (_thread.joinable()) {
             _thread.join() ;
          }
        });
  }
};

class thread_pool {
  using WorkerPool = std::vector<std::thread>;

  ThreadsafeQ<std::function<void ()>> tasks;
  WorkerPool workers;
  ThreadJoiner joiner; 
  std::atomic_bool  done;

  void worker_method() {
    while(!done) {
      auto work = tasks.wait_and_pop();
      work();
    }
  }
  public:
  thread_pool(const size_t total_threads = std::thread::hardware_concurrency()) : joiner(workers), done(false){
    WorkerPool(total_threads, std::thread(&thread_pool::worker_method));
  }


};