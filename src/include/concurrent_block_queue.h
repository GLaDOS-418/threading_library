#ifndef CONCURRENT_BLOCK_QUEUE_H
#define CONCURRENT_BLOCK_QUEUE_H

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

namespace ds {
  template <typename T, size_t BLOCK_SIZE = 512> class ConcurrentBlockQueue {
    struct Node;
    struct Head;
    struct Tail;

    Head m_head;
    mutable std::mutex m_head_lock;

    Tail m_tail;
    mutable std::mutex m_tail_lock;

    std::condition_variable cv;

    std::atomic<size_t> m_size;

    inline auto get_tail( ) const {
      std::lock_guard<std::mutex> guard(m_tail_lock);
      return std::pair{m_tail.get( ), m_tail.offset};
    }

    inline bool compare_head_tail( ) const {
      auto [tail_block,tail_offset] = get_tail();
      return m_head.get() == tail_block && m_head.offset == tail_offset;
    }

    public:

    ConcurrentBlockQueue()
      : m_head{.head_block = std::make_unique<Node>(), .offset = 0},
      m_tail{.tail_block = m_head.head_block.get(), .offset = m_head.offset} {
      }

    void push(T val) {
      std::lock_guard<std::mutex> guard(m_tail_lock);
      m_tail.add_data(std::forward<T>(val));
      ++m_size;
      cv.notify_one();
    }

    std::optional<T> try_pop() {
      std::lock_guard<std::mutex> guard(m_head_lock);
      if (compare_head_tail()) {
        return {};
      }

      --m_size;
      return {m_head.pop()};
    }

    T wait_and_pop() {
      std::unique_lock<std::mutex> guard(m_head_lock);
      cv.wait(guard, [&]() { return not compare_head_tail(); });

      --m_size;
      return {m_head.pop()};
    }

    bool empty() const {
      std::lock_guard<std::mutex> guard(m_head_lock);
      return compare_head_tail( );
    }

    size_t size( ) const {
      return m_size;
    }
  };

  template <typename T, size_t BLOCK_SIZE>
    struct ConcurrentBlockQueue<T, BLOCK_SIZE>::Node {
      std::vector<T> data;
      std::unique_ptr<Node> next = nullptr;

      Node() { data.reserve(BLOCK_SIZE); }
    };

  template <typename T, size_t BLOCK_SIZE>
    struct ConcurrentBlockQueue<T, BLOCK_SIZE>::Head {
      std::unique_ptr<Node> head_block;
      size_t offset;

      const Node *get() const { return head_block.get(); };

      T pop() {
        if (offset == head_block->data.capacity()) {
          head_block = std::move(head_block->next);
          offset = 0;
        }

        auto front = std::move(head_block->data[offset]);
        ++offset;

        return front;
      }
    };

  template <typename T, size_t BLOCK_SIZE>
    struct ConcurrentBlockQueue<T, BLOCK_SIZE>::Tail {
      Node *tail_block;
      size_t offset;

      const Node *get() const { return tail_block; }

      bool full() { return tail_block->data.capacity() == offset; }

      void add_data(T val) {
        if (full()) {
          extend_tail_block();
        }

        tail_block->data[offset] = std::move(val);
        ++offset;
      }

      private:
      void extend_tail_block() {
        auto new_tail = std::make_unique<Node>();
        tail_block->next = std::move(new_tail);
        tail_block = tail_block->next.get();
        offset = 0;
      }
    };
} // namespace ds


#endif // CONCURRENT_BLOCK_QUEUE_H
