#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H

#include <concepts>
#include <functional>
#include <future>
#include <type_traits>

namespace util {

  template <typename ResultT>
    requires ( std::copyable<ResultT> || std::movable<ResultT> )
  class AsyncResult {
    std::future<ResultT> m_waitable;

    public:

    AsyncResult( auto&& waitable ) : m_waitable{ std::move( waitable ) } { }
    AsyncResult( const auto& waitable ) : m_waitable{ waitable  } { }

    AsyncResult( ) = default;
    ~AsyncResult( ) = default;

    AsyncResult( const AsyncResult& ) = delete;
    AsyncResult& operator=( const AsyncResult& ) = delete;

    AsyncResult( AsyncResult&& ) = default;
    AsyncResult& operator=( AsyncResult&& ) = default;

    template <typename Fn, std::enable_if_t<!std::is_same_v<void, ResultT>, int> = 0>
      requires std::movable<Fn>
    inline auto then( Fn&& callback ) {
      using return_t = std::invoke_result_t<Fn,ResultT>;

      std::promise<return_t> prms;
      prms.set_value( std::move( callback(m_waitable.get( )) ) );

      return AsyncResult<return_t>{ std::move( prms.get_future() ) };
    }

    ResultT get( ) {
      if constexpr ( std::is_same_v<void, ResultT> ){
        m_waitable.get( );
        return;
      }

      return m_waitable.get();
    }

    void wait( ) {
      m_waitable.wait();
      return;
    }

    bool valid( ) const {
      return m_waitable.valid();
    }

  };
}




#endif //ASYNC_RESULT_H
