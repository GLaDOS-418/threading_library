# threading library

a threading library that is:
- simple 
- standalone
- header-only
- uses only standard C++

### features

#### data structures

##### [ds::BlockingDeque](./src/include/ds/blocking_deque.h)
- synchronised queue implemented using `std::deque`
- usage: `ds::BlockingDeque<std::string> sq;`
- **TODO**: make this just a wrapper to provide same functionalities as `std::deque`.

##### [ds::ConcurrentBlockQueue](./src/include/ds/concurrent_block_queue.h)
- fine-grained locking, FIFO structure. 
- with `BLOCK_SIZE=1`, it's essentially a queue based on singly linked-list. default is `BLOCK_SIZE=512`
- unless necessary, push & pop can work independently without blocking each other.
- usage :  `ds::ConcurrentBlockQueue<std::,256> bq;`
<img src="./resources/images/concurrent_blocked_queue.svg" alt="block_queue" style="max-width: 50%;"/>

#### utilities

##### [util::FunctionWrapper](./src/include/util/function_wrapper.h)
- a type erased function wrapper
- nothing to do with concurrency but, implemented so why not just use it.
- usage : `FunctionWrapper{ [ ]( ){ /*  do something in this lambda */; } };`


##### [util::ThreadPool](./src/include/util/thread_pool.h)
- a thread pool with customisable number of worker threads.
- default pool size is determined by `std::thread::hardware_concurrency( )`.
- during task submission, a `std::future<T>` is returned to make the results available later.
- usage : `ThreadPool tp(20);`


### build

- C++ version used: `-std=c++2b`
- compiler: `clang++`
- build manager: `cmake`
- package manager: `conan`
- testing framework: `catch2 v3`
- important compiler options: `-ggdb3`, `-fsanitize=undefined,address`

1. required : cmake, conan, clang, gdb
2. run `$ ./project_build.sh`


### test

1. run `$ make test`


### todo
- Add features like concurrent cache, concurrent stack, lock free queue, algorithms like zip etc.
- Setup environment in a dockerfile.
- Add github actions.
