#include <gtest/gtest.h>
#include <future>
#include <string>
#include <optional>

#include "DataStructures/ConcurrentBlockQueue.hpp"

TEST(ConcurrentBlockQueueTests, WhenCreatedShouldBeEmpty)
{
    DataStructures::ConcurrentBlockQueue<std::string> queue;

    EXPECT_TRUE(queue.was_empty());
    EXPECT_EQ(0U, queue.was_size());
}

TEST(ConcurrentBlockQueueTests, WhenItemPushedShouldPopSameValue)
{
    DataStructures::ConcurrentBlockQueue<std::string> queue;

    EXPECT_EQ(0U, queue.push(std::string("alpha")));

    auto value = queue.try_pop();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ("alpha", value.value());
    EXPECT_TRUE(queue.was_empty());
}

TEST(ConcurrentBlockQueueTests, WhenWaitingConsumerExistsShouldReceivePushedValue)
{
    DataStructures::ConcurrentBlockQueue<std::string> queue;

    auto pending = std::async(
        std::launch::async,
        [&queue]()
        {
            return queue.wait_and_pop();
        });

    EXPECT_EQ(0U, queue.push(std::string("beta")));

    auto value = pending.get();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ("beta", value.value());
}

TEST(ConcurrentBlockQueueTests, WhenClearModeEnabledShouldRejectPushAndUnblockWaiters)
{
    DataStructures::ConcurrentBlockQueue<std::string> queue;

    auto pending = std::async(
        std::launch::async,
        [&queue]()
        {
            return queue.wait_and_pop();
        });

    queue.enable_clear_mode();

    EXPECT_EQ(1U, queue.push(std::string("gamma")));

    auto value = pending.get();

    EXPECT_FALSE(value.has_value());
    EXPECT_TRUE(queue.was_empty());
}
