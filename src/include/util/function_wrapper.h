#ifndef FUNCTION_WRAPPER_H
#define FUNCTION_WRAPPER_H

#include <memory>
#include <type_traits>

class FunctionWrapper {
  struct ICallable {
    virtual void call() const = 0;
    virtual ~ICallable() = default;
  };

  template <typename T> 
  struct CallableImpl : ICallable {
    explicit CallableImpl(T&& f) : callable(std::move(f)) {}
    inline void call() const override { callable(); }

    private:
    T callable;
  };

  std::unique_ptr<ICallable> pImpl;

  public:

  template<typename F>
  FunctionWrapper( F&& callable) : 
    pImpl( std::make_unique<CallableImpl<F>>(std::move(callable)) )
  { };

  FunctionWrapper( const FunctionWrapper& ) = delete;
  FunctionWrapper& operator=( const FunctionWrapper& ) = delete;

  // can't be 'default'-ed due to collision with argument-based ctor above
  FunctionWrapper( FunctionWrapper&& other ) : pImpl(std::move(other.pImpl)){ }
  FunctionWrapper& operator=( FunctionWrapper&& other ) {
    pImpl.swap(other.pImpl);
    return *this;
  }

  ~FunctionWrapper( ) = default;

  void operator( ) ( ) {
    pImpl->call();
  }

};

#endif // FUNCTION_WRAPPER_H
