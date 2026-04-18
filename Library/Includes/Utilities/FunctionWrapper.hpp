#ifndef _LIBRARY_UTILITIES_FUNCTIONWRAPPER_HPP
#define _LIBRARY_UTILITIES_FUNCTIONWRAPPER_HPP

#include <functional>
#include <memory>
#include <utility>

namespace Utilities
{
    class FunctionWrapper
    {
        struct ICallable
        {
            virtual void call() = 0;
            virtual ~ICallable() = default;
        };

        template<typename T>
        struct CallableImpl : ICallable
        {
            explicit CallableImpl(T&& f)
                : callable(std::move(f))
            {
            }

            inline void call() override
            {
                callable();
            }

        private:
            T callable;
        };

        std::unique_ptr<ICallable> pImpl;

    public:
        /*
         *  perhaps, this implementation can act as type-erased wrapper for
         *  functions with arguments
         * */

        template<typename F, typename... Args>
        FunctionWrapper(F&& callable, Args&&... args)
            : pImpl(std::make_unique<CallableImpl<F>>(
                  std::move(std::bind(std::forward<F>(callable), std::forward<Args>(args)...))))
        {
        }

        template<typename F>
        FunctionWrapper(F&& callable)
            : pImpl(std::make_unique<CallableImpl<F>>(std::move(callable)))
        {
        }

        // can't be 'default'-ed due to collision with argument-based ctor above
        FunctionWrapper(FunctionWrapper&& other) noexcept
            : pImpl(std::move(other.pImpl))
        {
        }

        FunctionWrapper& operator=(FunctionWrapper&& other) noexcept
        {
            pImpl.swap(other.pImpl);
            return *this;
        }

        FunctionWrapper(const FunctionWrapper&) = delete;
        FunctionWrapper& operator=(const FunctionWrapper&) = delete;

        ~FunctionWrapper() = default;

        void operator()()
        {
            pImpl->call();
        }
    };
}  // namespace Utilities

#endif  // !_LIBRARY_UTILITIES_FUNCTIONWRAPPER_HPP
