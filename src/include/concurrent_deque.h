#ifndef CONCURRENT_DEQUE_H
#define CONCURRENT_DEQUE_H

#include <condition_variable>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace ds {

  template <typename Data>
    requires(std::copyable<Data> || std::movable<Data>)
  class ConcurrentDeque {
    std::deque<Data> data;
    mutable std::mutex lock;
    std::condition_variable conditional;

    auto pop_and_return() {
      auto retval = std::move(data.front());
      data.pop_front();
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
      data.emplace_back(val);
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

#endif // CONCURRENT_DEQUE_H
