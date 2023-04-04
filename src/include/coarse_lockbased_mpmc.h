#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <queue>
#include <optional>
#include <condition_variable>
#include <utility>
#include <memory>

template<typename Data>
class CoarseLockedThreadSafeQ {
  std::queue<std::unique_ptr<Data>> data;
  mutable std::mutex lock;
  std::condition_variable conditional;


  auto&& pop_and_return() {
    auto retval = std::move(data.front());
    data.pop();
    return std::move(retval);
  }

  public:

  std::optional<std::unique_ptr<Data>> try_pop() {
    std::lock_guard<std::mutex> guard(lock);
    if(data.empty())
      return {};

    return pop_and_return();
  }

  std::optional<std::unique_ptr<Data>> wait_and_pop() {
    std::unique_lock<std::mutex> guard(lock);
    conditional.wait(guard,[this]() {return !data.empty();});

    return pop_and_return();
  }

  void push(Data val) {
    auto ptr = std::make_unique<Data>(val);
    std::lock_guard<std::mutex> guard(lock);
    data.push(std::move(ptr));
    conditional.notify_one();
  }
  
  size_t size() const {
    std::lock_guard<std::mutex> guard(lock);
    return data.size();
  }

  bool empty() const {
    std::lock_guard<std::mutex> guard(lock);
    return data.empty();
  }

};

// int main ( ) {
//   CoarseLockedThreadSafeQ<int> q;
// 
//   std::atomic<int> counter{0};
//   std::atomic<int> thresh{10'000'000};
// 
//   auto p1 = std::jthread([&]() {
//         while(counter < thresh){
//         q.push(rand());
//         ++counter;
//       }
//     });
// 
//   auto p2 = std::jthread([&]() {
//         while(counter < thresh){
//         q.push(rand());
//         ++counter;
//       }
//     });
// 
//   auto c1 = std::jthread([&]() {
//         while(counter < thresh){
//           auto data = q.wait_and_pop();
//           ++counter;
//       }
//     });
// 
//   auto c2 = std::jthread([&]() {
//         while(counter < thresh){
//           auto data = q.try_pop();
//           ++counter;
//       }
//     });
// 
//   return 0;
// }
