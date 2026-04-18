#include <gtest/gtest.h>
#include <string>
#include <optional>

#include "DataStructures/ConcurrentStack.hpp"

TEST(ConcurrentStackTests, WhenBoundedStackFullShouldRejectAdditionalPush)
{
    DataStructures::ConcurrentStack<std::string, 2> stack;

    EXPECT_TRUE(stack.push("first"));
    EXPECT_TRUE(stack.push("second"));
    EXPECT_FALSE(stack.push("third"));
    EXPECT_EQ(2U, stack.was_size());
}

TEST(ConcurrentStackTests, WhenItemsPushedShouldPopInLifoOrder)
{
    DataStructures::ConcurrentStack<std::string> stack;

    EXPECT_TRUE(stack.push("first"));
    EXPECT_TRUE(stack.push("second"));

    auto top = stack.try_pop();

    ASSERT_TRUE(top.has_value());
    EXPECT_EQ("second", top.value());
    EXPECT_EQ("first", stack.wait_and_pop());
    EXPECT_TRUE(stack.was_empty());
}
