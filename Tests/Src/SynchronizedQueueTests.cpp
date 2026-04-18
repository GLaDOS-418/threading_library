#include <gtest/gtest.h>
#include <string>
#include <optional>

#include "DataStructures/SynchronizedQueue.hpp"

TEST(SynchronizedQueueTests, WhenItemsPushedShouldPopInFifoOrder)
{
    DataStructures::SynchronizedQueue<std::string> queue;

    queue.push(std::string("first"));
    queue.push(std::string("second"));

    auto first = queue.try_pop();
    auto second = queue.wait_and_pop();

    ASSERT_TRUE(first.has_value());
    EXPECT_EQ("first", first.value());
    EXPECT_EQ("second", second);
    EXPECT_TRUE(queue.was_empty());
}
