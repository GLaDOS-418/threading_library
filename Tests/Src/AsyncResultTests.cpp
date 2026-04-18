#include <future>
#include <string>

#include <gtest/gtest.h>

#include "Utilities/AsyncResult.hpp"

TEST(AsyncResultTests, WhenCallbacksChainedShouldReturnFinalValue)
{
    std::promise<std::string> promise;
    Utilities::AsyncResult<std::string> result(promise.get_future());

    promise.set_value("123");

    auto finalValue = result
                          .then([](const std::string& value) noexcept { return std::stoi(value); })
                          .then([](int value) noexcept { return std::to_string(value + 1); })
                          .get();

    EXPECT_EQ("124", finalValue);
}
