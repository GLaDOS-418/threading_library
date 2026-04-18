#ifndef _LIBRARY_UTILITIES_THREADPOOL_HPP
#define _LIBRARY_UTILITIES_THREADPOOL_HPP

#include <atomic>
#include <cstddef>
#include <functional>
#include <future>
#include <thread>
#include <type_traits>
#include <vector>
#include <optional>
#include <utility>

#include "Utilities/AsyncResult.hpp"
#include "DataStructures/ConcurrentBlockQueue.hpp"
#include "Utilities/FunctionWrapper.hpp"

namespace Utilities
{

    class ThreadPool
    {
        using WaitableTask = Utilities::FunctionWrapper;
        using TaskQueue = DataStructures::ConcurrentBlockQueue<WaitableTask>;
        using WorkerGroup = std::vector<std::jthread>;

        TaskQueue tasks;
        WorkerGroup workers;
        std::atomic_bool done{false};  // true = complete all remaining work & exit

        void worker_method()
        {
            while (!done)
            {
                // wait_and_pop requires interruptible conditional variable
                // to wake the threads up in case a join( ) request received
                // when there are no tasks available
                auto task = tasks.try_pop();

                if (task)
                    (*task)();

                // irrespective of availability of tasks,
                // give a chance to other threads
                std::this_thread::yield();
            }
        }

        static size_t compute_concurrency()
        {
            return std::thread::hardware_concurrency() + 1;
        }

    public:
        ThreadPool(const size_t total_workers = compute_concurrency())
            : done(false)
        {
            try
            {
                for (auto i = 0u; i < total_workers; ++i)
                    workers.emplace_back(std::jthread(&Utilities::ThreadPool::worker_method, this));
            }
            catch (...)
            {
                join();
                throw;
            }
        }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        ~ThreadPool()
        {
            join();
        }

        void join()
        {
            done = true;
        }

        inline size_t size() const
        {
            return workers.size();
        }

        template<typename Fn, typename... Args>
        auto submit(Fn callable, Args&&... args)
        {
            using return_t = std::invoke_result_t<Fn, Args...>;
            std::packaged_task<return_t()> task(std::bind(std::forward<Fn>(callable), std::forward<Args>(args)...));

            // caller waits on this future
            Utilities::AsyncResult<return_t> result{task.get_future()};
            tasks.push(std::move(task));

            return result;
        }
    };
}  // namespace Utilities

#endif  // !_LIBRARY_UTILITIES_THREADPOOL_HPP
