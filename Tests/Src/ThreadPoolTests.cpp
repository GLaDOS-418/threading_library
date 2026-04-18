#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "Utilities/ThreadPool.hpp"

TEST(ThreadPoolTests, WhenTaskSubmittedShouldReturnTaskResult)
{
    Utilities::ThreadPool pool(1);

    auto result = pool.submit([](int lhs, int rhs) noexcept { return lhs + rhs; }, 20, 22);

    EXPECT_EQ(42, result.get());
}

TEST(ThreadPoolTests, WhenMultipleTasksSubmittedShouldPreserveAllResults)
{
    Utilities::ThreadPool pool(2);
    std::vector<Utilities::AsyncResult<std::string>> results;

    results.emplace_back(pool.submit([]() noexcept { return std::string("one"); }));
    results.emplace_back(pool.submit([]() noexcept { return std::string("two"); }));

    EXPECT_EQ("one", results[0].get());
    EXPECT_EQ("two", results[1].get());
}
