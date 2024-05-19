#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <thread>

#include <ds/concurrent_block_queue.h>
#include <ds/concurrent_hash_map.h>
#include <ds/concurrent_stack.h>
#include <ds/synchronized_queue.h>

TEST_CASE("block_queue_wait_and_pop", "[ConcurrentDS]") {
  ds::ConcurrentBlockQueue<std::string, 1> blockQ;
  const size_t iters = 1'000'000;
  auto thread1 = std::thread([&]() {
    for (auto i = 0U; i < iters; ++i) {
      blockQ.push(std::to_string(i));
    }
  });

  auto thread2 = std::thread([&]() {
    int64_t sum = 0;
    for (auto i = 0U; i < iters; ++i) {
      auto data = blockQ.wait_and_pop();
      if (data) {
        sum += std::stoi(*data);
      }
    }

    REQUIRE((sum == 499'999'500'000));
  });

  thread1.join();
  thread2.join();

  REQUIRE((blockQ.was_size() == 0));
}

TEST_CASE("block_queue_try_pop", "[ConcurrentDS]") {
  ds::ConcurrentBlockQueue<std::string> blockQ;

  const size_t iters = 1'000'000;

  auto thread1 = std::thread([&]() {
    for (auto i = 0U; i < iters; ++i) {
      blockQ.push(std::to_string(i));
    }
  });

  auto thread2 = std::thread([&]() {
    int64_t sum = 0;
    auto i = 0U;
    while (i < iters) {
      auto data = blockQ.try_pop();
      if (data) {
        sum += std::stoi(*data);
        ++i;
      }
    }

    REQUIRE((sum == 499'999'500'000));
  });

  thread1.join();
  thread2.join();

  REQUIRE((blockQ.was_size() == 0));
}

TEST_CASE("std_queue_try_pop", "[ConcurrentDS]") {
  ds::SynchronizedQueue<std::string> synchronizedQ;

  const size_t iters = 1'000'000;

  auto thread1 = std::thread([&]() {
    for (auto i = 0U; i < iters; ++i) {
      synchronizedQ.push(std::to_string(i));
    }
  });

  auto thread2 = std::thread([&]() {
    int64_t sum = 0;
    auto i = 0U;
    while (i < iters) {
      auto data = synchronizedQ.try_pop();
      if (data) {
        sum += std::stoi(*data);
        INFO(sum);
        ++i;
      }
    }

    REQUIRE((sum == 499'999'500'000));
  });

  thread1.join();
  thread2.join();

  REQUIRE((synchronizedQ.was_size() == 0));
}

TEST_CASE("concurrent_hash_map", "[ConcurrentDS]") {
  ds::ConcurrentHashMap<std::string, std::string> cmap;

  const size_t iters = 100'000;
  auto thread1 = std::thread([&cmap]() {
    for (auto i = 0U; i < iters; ++i) {
      cmap.insert(std::to_string(i), std::to_string(i));
    }
  });

  auto thread2 = std::thread([&cmap]() {
    for (auto i = 0U; i < iters;) {
      auto data = cmap.remove(std::to_string(i));
      if (data) {
        i += 2;
      }
    }
  });

  auto thread3 = std::thread([&cmap]() {
    for (auto i = 1U; i < iters; ++i) {
      auto mapValue = cmap.get(std::to_string(i));
      auto expectedValue = std::to_string(i);

      if (mapValue) {
        REQUIRE(((*mapValue) == expectedValue));
      }
    }
  });

  thread1.join();
  thread2.join();
  thread3.join();

  REQUIRE(cmap.was_size() == (iters >> 1));
}

TEST_CASE("bounded_concurrent_stack_wait_pop", "[ConcurrentDS]") {
  ds::ConcurrentStack<std::string, 200> stck;

  const int iters = 100'000;

  auto thread1 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto success = stck.push(std::to_string(i));
      if (success) {
        ++i;
      }
    }
  });

  auto thread2 = std::thread([&stck]() {
    for (int i = 0; i < iters; ++i) {
      auto stackVal = stck.wait_and_pop();
    }
  });

  thread1.join();
  thread2.join();

  REQUIRE((stck.was_size() == 0));
}

TEST_CASE("bounded_concurrent_stack_try_pop", "[ConcurrentDS]") {
  ds::ConcurrentStack<std::string, 200> stck;

  const int iters = 100'000;

  auto thread1 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto success = stck.push(std::to_string(i));
      if (success) {
        ++i;
      }
    }
  });

  auto thread2 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto stackVal = stck.try_pop();
      if (stackVal) {
        ++i;
      }
    }
  });

  thread1.join();
  thread2.join();

  REQUIRE((stck.was_size() == 0));
}

TEST_CASE("unbounded_concurrent_stack_wait_pop", "[ConcurrentDS]") {
  ds::ConcurrentStack<std::string> stck;

  const int iters = 100'000;

  auto thread1 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto success = stck.push(std::to_string(i));
      if (success) {
        ++i;
      }
    }
  });

  auto thread2 = std::thread([&stck]() {
    for (int i = 0; i < iters; ++i) {
      auto stackVal = stck.wait_and_pop();
    }
  });

  thread1.join();
  thread2.join();

  REQUIRE((stck.was_size() == 0));
}

TEST_CASE("unbounded_concurrent_stack_try_pop", "[ConcurrentDS]") {
  ds::ConcurrentStack<std::string> stck;

  const int iters = 100'000;

  auto thread1 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto success = stck.push(std::to_string(i));
      if (success) {
        ++i;
      }
    }
  });

  auto thread2 = std::thread([&stck]() {
    for (int i = 0; i < iters;) {
      auto stackVal = stck.try_pop();
      if (stackVal) {
        ++i;
      }
    }
  });

  thread1.join();
  thread2.join();

  REQUIRE((stck.was_size() == 0));
}
