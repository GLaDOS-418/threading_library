#include "concurrent_std_queue.h"
#include "concurrent_block_queue.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <utility>

using namespace std;

void test_coarse_lockbased_mpmc() {
  ds::ConcurrentStdQueue<int> q;

  std::atomic<int> counter{0};
  std::atomic<int> thresh{10'000'000};

  auto p1 = std::jthread([&]() {
    while (counter < thresh) {
      q.push(rand());
      ++counter;
    }
  });

  auto p2 = std::jthread([&]() {
    while (counter < thresh) {
      q.push(rand());
      ++counter;
    }
  });

  auto c1 = std::jthread([&]() {
    while (counter < thresh) {
      auto data = q.wait_and_pop();
      ++counter;
    }
  });

  auto c2 = std::jthread([&]() {
    while (counter < thresh) {
      auto data = q.try_pop();
      ++counter;
    }
  });
}

void test_block_queue_wait_and_pop(size_t n) {
  ds::ConcurrentBlockQueue<int, 2> bq;
  //BlockQueue<int> bq;
  // int n = 10;
  //  std::cin >> n;
  auto t1 = std::jthread([&]() {
    for (auto i = 0u; i < n; ++i)
      bq.push(i);
  });

  auto t2 = std::jthread([&]() {
    size_t sum = 0;
    for (auto i = 0u; i < n; ++i) {
      auto data = bq.wait_and_pop();
      // if(data){
      sum += data; // std::cout << *data << ", ";
                   // std::cin >> n;
      //}
    }
    std::cout << "n: " << n << "  sum: " << sum << std::endl;
  });
}

void test_block_queue_try_pop(size_t n) {
  ds::ConcurrentBlockQueue<int, 2> bq;
  //BlockQueue<int> bq;
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
        sum += *data; // std::cout << *data << ", ";
        ++i;
      }
    }
    std::cout << "n: " << n << "  sum: " << sum << std::endl;
  });
}
int main(int argc, char *argv[]) {
  // test_coarse_lockbased_mpmc();

  if (argc > 1){
    test_block_queue_wait_and_pop(atoi(argv[1]));
    test_block_queue_try_pop(atoi(argv[1]));
  }
  return 0;
}
