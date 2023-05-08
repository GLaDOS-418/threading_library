# threading library

A simple threading library written in C++20 that is standalone (no third-party) and header-only.
The intent is to provide a starting point for a concurrent code.

***Copy the `include` directory in your project to use it.***


### features

- [data structures](#data-structures)
    - [concurrent hashmap](#concurrent-hashmap)
    - [concurrent block queue](#concurrent-block-queue)
    - [synchronized queue](#synchronized-queue)
    - [concurrent stack](#concurrent-stack)
- [utilities](#utilities)
    - [function wrapper](#function-wrapper)
    - [thread-pool](#thread-pool)
    - [async result](#async-result)

#### DATA STRUCTURES <a name="data-structures"/>

##### [ds::ConcurrentHashMap](./include/ds/concurrent_hash_map.h) <a name="concurrent-hashmap"/>
- bucket-level locking based, concurrent hash map.
- number of buckets can be adjusted with a template parameter, default is `BUCKETS=1031`.
- below example creates a hash map with (key=std::string, val=double, buckets=513)
- usage : `ds::ConcurrentHashMap<std::string,double,513>`

##### [ds::ConcurrentBlockQueue](./include/ds/concurrent_block_queue.h) <a name="concurrent-block-queue"/>
- fine-grained locking, FIFO-queue. 
- with `BLOCK_SIZE=1`, it's essentially a queue based on singly linked-list. default is `BLOCK_SIZE=512`
- unless necessary, push & pop can work independently without blocking each other.
- usage :  `ds::ConcurrentBlockQueue<std::string,256> bq;`
<img src="./resources/images/concurrent_blocked_queue.svg" alt="block_queue" style="max-width: 50%;"/>

##### [ds::SynchronizedQueue](./include/ds/synchronized_queue.h) <a name="synchronized-queue"/>
- coarse-grained synchronized FIFO-queue, implemented using `std::deque`
- blocks the whole structure for both push & pop.
- usage: `ds::SynchronizedQueue<std::string> sq;`

##### [ds::ConcurrentStack](./include/ds/concurrent_stack.h) <a name="concurrent-stack"/>
- lock-based bounded( `container: std::vector` ) and unbounded( `container:std::deque` ) LIFO structure.
- since, only one point of access, no option other than locking the whole structure is available.
- usage [bounded stack]   : `ds::ConcurrentStack<value_type,bound_size>`
- usage [unbounded stack] : `ds::ConcurrentStack<value_type>`

#### UTILITIES

##### [util::FunctionWrapper](./include/util/function_wrapper.h) <a name="function-wrapper"/>
- a type erased function wrapper
- usage : `util::FunctionWrapper{ callable };`

##### [util::AsyncResult](./include/util/async_result.h) <a name="async-result"/>
- a wrapper over `std::future` that allows chaining of callbacks once the result is available.
- usage : `util::AsyncResult<callback_return_type> result;`
- usage [chaining] : `auto final_result = result.then( f ).then( g ).then( h ).get( );`


##### [util::ThreadPool](./include/util/thread_pool.h) <a name="thread-pool"/>
- a thread pool with customisable number of worker threads.
- default pool size is determined by `std::thread::hardware_concurrency( )`.
- worker threads busy-waits for work.
- task submission returns a `util::AsyncResult<callback_return_t>` object.
- usage : `util::ThreadPool tp(20);`
- usage [submit task] : `auto result = tp.submit( callable );`


### build

- C++ version used: `-std=c++20`
- compiler: `clang++`
- build manager: `cmake`
- package manager: `conan`
- testing framework: `catch2 v3`
- important compiler options: `-ggdb3`, `-fsanitize=undefined,address`

1. required : cmake, conan, clang, gdb
2. run `$ ./project_build.sh`


### test

1. run `$ make test`
2. run `$ make testout` to re-run failed tests w/ output.


### todo
- [ ] lock free data structures stack, queue & cache.
- [ ] concurrent algorithms like zip.
- [ ] utilities like guarded resource, spin lock, seqlock, ticket lock.
- [ ] homogenize container interface using concepts.
- [ ] setup environment in a dockerfile.
- [ ] add github actions.
- [ ] add benchmark.
- [ ] improve documentation e.g. add code examples etc.
