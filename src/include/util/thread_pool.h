#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <functional>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>

#include <ds/concurrent_block_queue.h>
#include <util/function_wrapper.h>

namespace util {

  class ThreadPool {
    using WaitableTask =  FunctionWrapper;
    using TaskQueue    =  ds::ConcurrentBlockQueue<WaitableTask>;
    using WorkerGroup  =  std::vector<std::jthread>;

    TaskQueue   tasks;
    WorkerGroup workers;
    std::atomic_bool done{false}; // true = complete all remaining work & exit

    void worker_method() {
      while (!done) {
        auto task = tasks.wait_and_pop();
        if(task)
          (*task)( );
      }
    }

    static size_t compute_concurrency( ) {
      return std::thread::hardware_concurrency() + 1;
    }

    public:

    void join( ){
      done = true;
      tasks.enable_clear_mode();
    }

    ThreadPool(const size_t total_workers = compute_concurrency( )) : done(false) { 
      try{ 
        for( auto i=0u; i<total_workers; ++i)
          workers.emplace_back( std::jthread(&util::ThreadPool::worker_method, this) );
      }
      catch(...){
        join();
        throw ;
      }
    }

    ThreadPool( const ThreadPool& ) = delete;
    ThreadPool& operator=( const ThreadPool& ) = delete;

    ThreadPool( ThreadPool&& ) = delete;
    ThreadPool& operator=( ThreadPool&& ) = delete;

    ~ThreadPool( ){
      join();
    }

    template<typename Fn, typename... Args>
    std::future<std::invoke_result_t<Fn, Args...>> submit( Fn callable, Args&&... args){
      using return_t = std::invoke_result_t<Fn, Args...>;
      std::packaged_task<return_t( )> task(std::bind(std::forward<Fn>(callable), std::forward<Args>(args)...));

      auto result{task.get_future( )};
      tasks.push(std::move(task));

      return result;
    }

  };
}; // namespace utility

#endif // THREAD_POOL_H
