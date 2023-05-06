#ifndef CONCURRENT_STACK_H
#define CONCURRENT_STACK_H

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <type_traits>
#include <vector>
#include <deque>
#include <optional>


namespace ds {

  template<typename Data, size_t ContainerSize = 0>
  class ConcurrentStack {

    using unbounded_container = std::deque<Data>;
    using   bounded_container = std::vector<Data>;

    std::conditional_t<(ContainerSize>0), bounded_container, unbounded_container> m_stack;
    std::atomic<size_t> m_size;

    // synchronization primitives
    std::mutex m_lock;
    std::condition_variable m_sync;

    public:

    std::optional<Data> try_pop( ) {
      std::lock_guard<std::mutex> guard(m_lock);
      if( was_empty() )
        return { };

      auto retval = std::move( m_stack.back( ) );

      m_stack.pop_back( );
      --m_size;

      return { std::move(retval) };
    }

    Data wait_and_pop( ){
      std::unique_lock<std::mutex> guard(m_lock);
      m_sync.wait(guard, [this]( ){ return not was_empty( ); });

      auto retval = std::move( m_stack.back( ) );

      --m_size;
      m_stack.pop_back( );

      return retval;
    }

    bool push( Data value ) {
      std::lock_guard<std::mutex> guard(m_lock);

      if constexpr ( ContainerSize > 0 ){
        if ( m_size == ContainerSize ) // container full. can't push new data
          return false;
      }

      m_stack.emplace_back( std::move(value) );
      ++m_size;
      m_sync.notify_one();

      return true; // success
    }

    bool was_empty( ) const {
      return !m_size;
    }

    size_t was_size( ) const {
      return m_size;
    }

  };

} // ds

#endif // CONCURRENT_STACK_H
