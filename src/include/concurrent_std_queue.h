#include <condition_variable>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>

namespace ds {

  template <typename Data> class ConcurrentStdQueue {
    std::queue<Data> data;
    mutable std::mutex lock;
    std::condition_variable conditional;

    auto pop_and_return() {
      auto retval = std::move(data.front());
      data.pop();
      return retval;
    }

    public:
    std::optional<Data> try_pop() {
      std::lock_guard<std::mutex> guard(lock);
      if (data.empty())
        return {};

      return pop_and_return();
    }

    Data wait_and_pop() {
      std::unique_lock<std::mutex> guard(lock);
      conditional.wait(guard, [this]() { return !data.empty(); });

      return pop_and_return();
    }

    void push(Data val) {
      std::lock_guard<std::mutex> guard(lock);
      data.emplace(val);
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

} // namespace ds
