#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>

#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <thread>

#include "concurrent_block_queue.h"
#include "concurrent_std_queue.h"

TEST_CASE("block_queue_wait_and_pop", "[ConcurrentDatastructures]") {
  ds::ConcurrentBlockQueue<size_t,1> bq;
  size_t n = 1'000'000;
  auto t1 = std::jthread([&]() {
    for (auto i = 0u; i < n; ++i)
      bq.push(i);
  });

  auto t2 = std::jthread([&]() {
    size_t sum = 0;
    for (auto i = 0u; i < n; ++i) {
      auto data = bq.wait_and_pop();
      sum += data;
    }

    REQUIRE(sum == 499'999'500'000);
  });
  
  REQUIRE(bq.size() == 0);
}


TEST_CASE("block_queue_try_pop", "[ConcurrentDatastructures]") {
  ds::ConcurrentBlockQueue<size_t> bq;

  size_t n = 1'000'000;

  auto t1 = std::jthread([&]() {
    for (auto i = 0u; i < n; ++i)
      bq.push(i);
  });

  auto t2 = std::jthread([&]() {
    size_t sum = 0;
    auto i = 0u;
    while (i < n) {
      auto data = bq.try_pop();
      if (data) {
        sum += *data;
        ++i;
      }
    }

    REQUIRE(sum == 499'999'500'000);
  });

  REQUIRE(bq.size() == 0);
}

TEST_CASE("std_queue_try_pop", "[ConcurrentDatastructures]") {
  ds::ConcurrentStdQueue<size_t> sq;

  size_t n = 1'000'000;

  auto t1 = std::jthread([&]() {
    for (auto i = 0u; i < n; ++i)
      sq.push(i);
  });

  auto t2 = std::jthread([&]() {
    size_t sum = 0;
    auto i = 0u;
    while (i < n) {
      auto data = sq.try_pop();
      if (data) {
        sum += *data;
        INFO( sum );
        ++i;
      }
    }

    REQUIRE(sum == 499'999'500'000);
  });

  REQUIRE(sq.size() == 0);
}
