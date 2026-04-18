#include <cstdlib>
#include <iostream>
#include <string>

#include "DataStructures/ConcurrentBlockQueue.hpp"
#include "Utilities/ThreadPool.hpp"

int main()
{
    DataStructures::ConcurrentBlockQueue<std::string> queue;
    const std::string value = "example-value";

    if (0U != queue.push(std::string(value)))
    {
        return EXIT_FAILURE;
    }

    auto popped = queue.try_pop();
    if (!popped.has_value() || value != popped.value())
    {
        return EXIT_FAILURE;
    }

    Utilities::ThreadPool pool(1);
    auto result = pool.submit([]() noexcept { return 42; });
    if (42 != result.get())
    {
        return EXIT_FAILURE;
    }

    std::cout << "threading_library smoke app completed successfully\n";
    return EXIT_SUCCESS;
}
