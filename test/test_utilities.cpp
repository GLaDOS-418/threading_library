#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <iterator>
#include <string>
#include <thread>
#include <future>

#include <util/thread_pool.h>

TEST_CASE("waitable_task_thread_pool", "[ConcurrentUtilities]") {
  size_t n = 100'000;
  util::ThreadPool tp;
  std::vector<util::AsyncResult<std::string>> futures;

  for(auto i=0u;i<n;++i){
    auto fut = tp.submit( [](int x)->std::string{ return std::to_string(x); }, i );
    futures.emplace_back(std::move(fut));
  }

  for(auto i = 0u; i<n; ++i ){
    auto exp = std::to_string( i ) ;
    auto res = futures[i]
       .then([ ]( std::string _i ) { return std::stoi(_i); })
       .then([ ]( int _i ) { return std::to_string(_i); } )
       .get( );

    REQUIRE( ( res == exp ) );
  }
}
