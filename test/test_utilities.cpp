
#include "util/function_wrapper.h"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <iterator>
#include <thread>
#include <future>

#include <util/thread_pool.h>

TEST_CASE("waitable_task_thread_pool", "[ConcurrentUtilities]") {
  size_t n = 100'000;
  util::ThreadPool tp;
  std::vector<std::future<std::string>> futures;

  for(auto i=0u;i<n;++i){
    auto fut = tp.submit( [](int x)->std::string{ return std::to_string(x<<1); }, i );
    futures.emplace_back(std::move(fut));
  }

  for(auto i = 0u; i<n; ++i ){
    auto res = futures[i].get();
    auto exp = std::to_string( i<<1 ) ;
    REQUIRE( ( res == exp ) );
  }
}
