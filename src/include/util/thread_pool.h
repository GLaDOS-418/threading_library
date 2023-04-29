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

namespace utility {

  class thread_pool {
    using WaitableTask =  FunctionWrapper;
    using TaskQueue    =  ds::ConcurrentBlockQueue<WaitableTask>;
    using WorkerGroup  =  std::vector<std::jthread>;

    TaskQueue   tasks;
    WorkerGroup workers;
    std::atomic_bool done{false}; // true = complete all remaining work & exit

    void worker_method() {
      while (!done) {
        tasks.wait_and_pop()( );
      }
    }

    static size_t compute_concurrency( ) {
      return std::thread::hardware_concurrency() + 1;
    }

    public:

    void shutdown( ){
      done = false;
    }

    thread_pool(const size_t total_workers = compute_concurrency( )) : done(false) { 
      try{ 
        for( auto i=0u; i<total_workers; ++i)
          workers.emplace_back( std::jthread(&utility::thread_pool::worker_method, this) );
      }
      catch(...){
        shutdown();
        throw ;
      }
    }

    thread_pool( const thread_pool& ) = delete;
    thread_pool& operator=( const thread_pool& ) = delete;

    thread_pool( thread_pool&& ) = delete;
    thread_pool& operator=( thread_pool&& ) = delete;

    ~thread_pool( ){
      shutdown();
    }

    template<typename Fn>
    auto submit( Fn callable ){
      std::packaged_task<std::invoke_result_t<Fn( )>> task(std::move(callable));

      auto result{task.get_future( )};
      tasks.push(std::move(callable));

      return result;
    }

  };
}; // namespace utility

#endif // THREAD_POOL_H
