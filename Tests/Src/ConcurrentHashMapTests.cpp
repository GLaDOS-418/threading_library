#include <gtest/gtest.h>
#include <string>
#include <optional>

#include "DataStructures/ConcurrentHashMap.hpp"

TEST(ConcurrentHashMapTests, WhenValueInsertedShouldBeReadableAndRemovable)
{
    DataStructures::ConcurrentHashMap<std::string, std::string> map;

    map.insert(std::string("key"), std::string("value"));

    auto stored = map.get("key");
    ASSERT_TRUE(stored.has_value());
    EXPECT_EQ("value", stored.value());
    EXPECT_EQ(1U, map.was_size());

    auto removed = map.remove("key");
    ASSERT_TRUE(removed.has_value());
    EXPECT_EQ("value", removed.value());
    EXPECT_TRUE(map.was_empty());
}
