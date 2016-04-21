#include "gtest/gtest.h"
#include "infra/util/public/Compare.hpp"

TEST(OperatorsTest, TestEqualityComparable)
{
    struct MyStruct
        : infra::EqualityComparable<MyStruct>
    {
        bool operator==(const MyStruct& other) const { return true; }
    };

    EXPECT_FALSE(MyStruct() != MyStruct());
}

TEST(OperatorsTest, TestLessThanComparable)
{
    struct MyStruct
        : infra::LessThanComparable<MyStruct>
    {
        bool operator<(const MyStruct& other) const { return true; }
    };

    EXPECT_TRUE(MyStruct() > MyStruct());
    EXPECT_FALSE(MyStruct() <= MyStruct());
    EXPECT_FALSE(MyStruct() >= MyStruct());
}

TEST(OperatorsTest, TestTotallyOrdered)
{
    struct MyStruct
        : infra::TotallyOrdered<MyStruct>
    {
        bool operator==(const MyStruct& other) const { return true; }
        bool operator<(const MyStruct& other) const { return true; }
    };

    EXPECT_FALSE(MyStruct() != MyStruct());
    EXPECT_TRUE(MyStruct() > MyStruct());
    EXPECT_FALSE(MyStruct() <= MyStruct());
    EXPECT_FALSE(MyStruct() >= MyStruct());
}

TEST(OperatorsTest, TestEqualityComparableHeterogeneous)
{
    struct MyStruct
        : infra::EqualityComparableHeterogeneous<MyStruct, std::nullptr_t>
    {
        bool operator==(std::nullptr_t) const { return true; }
    };

    EXPECT_FALSE(MyStruct() != nullptr);

    EXPECT_TRUE(nullptr == MyStruct());
    EXPECT_FALSE(nullptr != MyStruct());
}

TEST(OperatorsTest, TestLessThanComparableHeterogeneous)
{
    struct MyStruct
        : infra::LessThanComparableHeterogeneous<MyStruct, std::nullptr_t>
    {
        bool operator<(std::nullptr_t) const { return true; }
        bool operator>(std::nullptr_t) const { return true; }
    };

    EXPECT_FALSE(MyStruct() <= nullptr);
    EXPECT_FALSE(MyStruct() >= nullptr);

    EXPECT_TRUE(nullptr < MyStruct());
    EXPECT_TRUE(nullptr > MyStruct());
    EXPECT_FALSE(nullptr <= MyStruct());
    EXPECT_FALSE(nullptr >= MyStruct());
}

TEST(OperatorsTest, TestTotallyOrderedHeterogeneous)
{
    struct MyStruct
        : infra::TotallyOrderedHeterogeneous<MyStruct, std::nullptr_t>
    {
        bool operator==(std::nullptr_t) const { return true; }
        bool operator<(std::nullptr_t) const { return true; }
        bool operator>(std::nullptr_t) const { return true; }
    };

    EXPECT_FALSE(MyStruct() != nullptr);

    EXPECT_TRUE(nullptr == MyStruct());
    EXPECT_FALSE(nullptr != MyStruct());

    EXPECT_FALSE(MyStruct() <= nullptr);
    EXPECT_FALSE(MyStruct() >= nullptr);

    EXPECT_TRUE(nullptr < MyStruct());
    EXPECT_TRUE(nullptr > MyStruct());
    EXPECT_FALSE(nullptr <= MyStruct());
    EXPECT_FALSE(nullptr >= MyStruct());
}
