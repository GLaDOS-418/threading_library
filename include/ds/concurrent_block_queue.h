#ifndef CONCURRENT_BLOCK_QUEUE_H
#define CONCURRENT_BLOCK_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <vector>
#include <functional>

namespace ds {
  template <typename T, size_t BLOCK_SIZE = 512> 
    requires ( std::copyable<T> || std::movable<T> )
    class ConcurrentBlockQueue {

    /////////////////////////////////////////////
    ///  PRIVATE DATA STRUCTURES
    /////////////////////////////////////////////
    struct Node {
      std::vector<T> data = {};
      std::unique_ptr<Node> next = nullptr;

      Node( ) { data.reserve(BLOCK_SIZE); }
      ~Node( ) = default;
      Node( Node&& ) = default;
      Node& operator=( Node&& ) = default;

      Node( const Node& ) = delete;
      Node& operator=( const Node& ) = delete;
    };

    struct Head{
      std::unique_ptr<Node> head_block = nullptr;
      size_t block_offset = 0;
      std::mutex lock;

      T pop_data( ) {
        auto retval = std::move(head_block->data[block_offset]); 
        update_head();
        return retval;
      }

      void update_head( ) {
        ++block_offset;
        if( block_offset == BLOCK_SIZE ){

          // save the next block as head is going to be deleted
          // first before it gets reassigned and keeping next
          // linked to head will delete that as well.
          auto next = std::move(head_block->next);
          head_block = std::move(next);

          block_offset = 0;
        }
      }

      ~Head( ){
        // delete last node(empty) for queue destruction
        auto next = std::move(head_block->next);
        head_block = std::move(next);
      }

      Head( const Head& ) = delete;
      Head& operator=( const Head& ) = delete;
      Head( Head&& ) = default;
      Head& operator=( Head&& ) = default;
      Head( std::unique_ptr<Node> _head ) : 
        head_block(std::move( _head )), block_offset(0)
      { }

    };

    struct Tail{
      Node* tail_block = nullptr;
      size_t block_offset = 0;
      std::mutex lock;

      Tail ( ) = default;
      ~Tail( ) = default;
      Tail( const Tail& ) = delete;
      Tail& operator=( const Tail& ) = delete;
      Tail( Tail&& ) = default;
      Tail& operator=( Tail&& ) = default;


      void update_tail( ) {
        ++block_offset;
        if( block_offset == BLOCK_SIZE ) { 
          auto next = std::make_unique<Node>();
          tail_block->next = std::move(next);
          tail_block = (tail_block->next).get( );
          block_offset = 0;
        }
      }

      inline void add_data( T&& val) {
        tail_block->data.emplace_back(std::move(val));
        update_tail();
      }
    };

    /////////////////////////////////////////////
    ///  PRIVATE DATA MEMBERS
    /////////////////////////////////////////////
    Head m_head;
    Tail m_tail;

    std::atomic<size_t> m_size{0};
    std::condition_variable m_queue_signal;

    /*
     * - controlled by client
     *   true = no push, no wait, only pop
     *   false = push and pop available
     */
    std::atomic<bool> m_clear_mode_enabled{false}; 

    /////////////////////////////////////////////
    ///  PUBLIC API
    /////////////////////////////////////////////

    public:

    ConcurrentBlockQueue ( ) :
      m_head( std::move(std::make_unique<Node>( ) ) )
    {
      m_tail.tail_block = m_head.head_block.get();
      m_tail.block_offset = m_head.block_offset;
    }

    ConcurrentBlockQueue ( const ConcurrentBlockQueue& ) = delete;
    ConcurrentBlockQueue& operator= ( const ConcurrentBlockQueue& ) = delete;

    ConcurrentBlockQueue ( ConcurrentBlockQueue&& ) = default;
    ConcurrentBlockQueue& operator= ( ConcurrentBlockQueue&& ) = default;
    ~ConcurrentBlockQueue( ) = default;

    /*  
     *  retval: 0= SUCCESS, 1= FAILED
     */
    size_t push( T&& val ){
      std::lock_guard<std::mutex> guard(m_tail.lock);
      if(m_clear_mode_enabled) return 1;
      m_tail.add_data(std::move(val));
      ++m_size;
      m_queue_signal.notify_one();

      return 0; // success
    }

    std::optional<T> try_pop( ) {
      std::lock_guard<std::mutex> guard(m_head.lock);
      if( not empty() ){
        auto data = m_head.pop_data( );
        --m_size;
        return {std::move(data)};
      }

      return { };
    }

    std::optional<T> wait_and_pop( ) {
      std::unique_lock<std::mutex> guard(m_head.lock);
      m_queue_signal.wait(guard, [this]( ){ return m_clear_mode_enabled || not empty( ); } );

      if(m_clear_mode_enabled && empty( ))
        return { };

      auto data{m_head.pop_data( )};
      --m_size;

      return data;
    }

    void enable_clear_mode( ) {
      m_clear_mode_enabled=true;
      m_queue_signal.notify_all( );
    }

    bool empty( ){
      return m_size == 0;
    }

    size_t size( ) {
      return m_size;
    }

  };
} // namespace ds


#endif // CONCURRENT_BLOCK_QUEUE_H
