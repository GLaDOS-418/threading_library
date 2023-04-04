#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <memory>

#include <iostream>
#include <string>
#include <vector>

template<typename T>
class ThreadsafeQ {
  mutable std::mutex lock; 
  std::condition_variable cv;

  std::queue<std::shared_ptr<T>> data;

  public:
  ThreadsafeQ() = default;
  ThreadsafeQ(const ThreadsafeQ&) = delete;
  ThreadsafeQ& operator=(const ThreadsafeQ&) = delete;
  
  ThreadsafeQ(ThreadsafeQ&&) = delete;
  ThreadsafeQ& operator=(ThreadsafeQ&&) = delete;

  void push(T val) {
    auto newNode = std::make_shared<T>(std::move(val));
    std::lock_guard<std::mutex> guard(lock);
    data.emplace(std::move(newNode));
    cv.notify_one();
  }


  T try_pop() {
    std::lock_guard<std::mutex> guard(lock);
    if(data.empty()) return nullptr;

    auto out = std::make_shared<T>(data.front().get());
    data.pop();

    return std::move(*out);
  }

  T wait_and_pop(){
    std::unique_lock<std::mutex> guard(lock);
    cv.wait(guard, [&](){ return !data.empty(); });

    auto out = data.front();
    data.pop();
    return std::move(*out);
  }

  bool empty() const {
    std::lock_guard<std::mutex> guard(lock);
    return data.empty();
  }

  std::size_t size() const {
    std::lock_guard<std::mutex> guard(lock);
    return data.size();
  }

};
