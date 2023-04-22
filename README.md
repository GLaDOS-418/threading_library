# Threading Library
---

A simple threading library.


### Features

1. [ds::BlockingDeque](./src/include/ds/blocking_deque.h)
    - synchronised `std::deque`
    - usage: `ds::BlockingDeque<std::string> sq;`

2. [ds::ConcurrentBlockQueue](./src/include/ds/concurrent_block_queue.h)
    - fine-grained lock-based, FIFO structure. 
    - with `BLOCK_SIZE=1`, it's essentially a queue based on singly linked-list. default is `BLOCK_SIZE=512`
    - unless necessary, push & pop can be independently without blocking each other.
    - usage :  `ds::ConcurrentBlockQueue<std::,256> bq;`

<img src="./resources/images/concurrent_blocked_queue.svg" alt="block_queue" style="max-width: 70%;"/>


### Build

- C++ version used: `-std=gnu++23`
- compiler: `clang++`
- build manager: `cmake`
- package manager: `conan`
- testing framework: `catch2 v3`
- important compiler options: `-ggdb3`, `-fsanitize=undefined,address`

1. required : cmake, conan, clang, gdb
2. run `$ ./project_build.sh`


### Test

1. run `$ make test`


### TODO
- Add more features, like thread pool, concurrent cache, lock free queue etc.
- Setup environment in a dockerfile.
- Add github actions.
