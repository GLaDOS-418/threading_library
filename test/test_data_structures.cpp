#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <iterator>
#include <thread>

#include <ds/concurrent_block_queue.h>
#include <ds/concurrent_hash_map.h>
#include <ds/blocking_deque.h>

TEST_CASE("block_queue_wait_and_pop", "[ConcurrentDS]") {
  ds::ConcurrentBlockQueue<std::string,1> bq;
  size_t n = 1'000'000;
  auto t1 = std::thread([&]() {
    for (auto i = 0u; i < n; ++i)
      bq.push(std::to_string(i));
  });

  auto t2 = std::thread([&]() {
    long sum = 0;
    for (auto i = 0u; i < n; ++i) {
      auto data = bq.wait_and_pop();
      if(data)
        sum += std::stoi(*data);
    }

    REQUIRE(sum == 499'999'500'000);
  });
  
  t1.join();
  t2.join();

  REQUIRE(bq.size() == 0);
}

TEST_CASE("block_queue_try_pop", "[ConcurrentDS]") {
  ds::ConcurrentBlockQueue<std::string> bq;

  size_t n = 1'000'000;

  auto t1 = std::thread([&]() {
    for (auto i = 0u; i < n; ++i)
      bq.push(std::to_string(i));
  });

  auto t2 = std::thread([&]() {
    long sum = 0;
    auto i = 0u;
    while (i < n) {
      auto data = bq.try_pop();
      if (data) {
        sum += std::stoi(*data);
        ++i;
      }
    }

    REQUIRE(sum == 499'999'500'000);
  });

  t1.join();
  t2.join();

  REQUIRE(bq.size() == 0);
}

TEST_CASE("std_queue_try_pop", "[ConcurrentDS]") {
  ds::BlockingDeque<std::string> sq;

  size_t n = 1'000'000;

  auto t1 = std::thread([&]() {
    for (auto i = 0u; i < n; ++i)
      sq.push(std::to_string(i));
  });

  auto t2 = std::thread([&]() {
    long sum = 0;
    auto i = 0u;
    while (i < n) {
      auto data = sq.try_pop();
      if (data) {
        sum += std::stoi(*data);
        INFO( sum );
        ++i;
      }
    }

    REQUIRE(sum == 499'999'500'000);
  });

  t1.join();
  t2.join();

  REQUIRE(sq.size() == 0);
}


TEST_CASE("concurrent_hash_map", "[ConcurrentDS]") {
  ds::ConcurrentHashMap<std::string, std::string> cmap;

  size_t n = 100'000;
  auto t1 = std::thread( [&cmap,n]( ){
    for( auto i=0u; i<n; ++i) { 
      cmap.insert(std::to_string(i), std::to_string(i));
    }
  });

  auto t2 = std::thread( [&cmap,n]( ){
    for( auto i=0u; i<n; ) { 
      auto data = cmap.remove(std::to_string(i));
      if(data)
        i += 2;
    }
  });

  auto t3 = std::thread( [&cmap,n]( ){
    for( auto i=1u; i<n; ++i) { 
      auto mapValue = cmap.get(std::to_string(i));
      auto expectedValue = std::to_string(i);

      if(mapValue){
        REQUIRE( (*mapValue) == expectedValue);
      }
    }
  });

  t1.join();
  t2.join();
  t3.join();

  REQUIRE( cmap.was_size( ) == (n>>1) );
}
