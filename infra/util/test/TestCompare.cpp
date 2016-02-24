#include "gtest/gtest.h"
#include "infra/util/public/Compare.hpp"

TEST(EqualsTest, TestConstructedEmpty)
{
    EXPECT_TRUE(infra::Equals());
}

TEST(EqualsTest, TestEqual1Bool)
{
    EXPECT_TRUE(infra::Equals()
        (true, true));
}

TEST(EqualsTest, TestEqual1Bool1Int)
{
    EXPECT_TRUE(infra::Equals()
        (true, true)
        (5, 5));
}

TEST(EqualsTest, TestNotEqual1Bool)
{
    EXPECT_FALSE(infra::Equals()
        (true, false));
}

TEST(EqualsTest, TestNotEqual1Bool1Int)
{
    EXPECT_FALSE(infra::Equals()
        (true, true)
        (5, 4));
}

TEST(EqualsTest, TestNotEqual1Bool1Int2)
{
    EXPECT_FALSE(infra::Equals()
        (true, false)
        (5, 5));
}

TEST(LessThanTest, TestConstructedEmpty)
{
    EXPECT_FALSE(infra::LessThan());
}

TEST(LessThanTest, TestLessThanBool)
{
    EXPECT_TRUE(infra::LessThan()
        (false, true));
}

TEST(LessThanTest, TestLessThanInt)
{
    EXPECT_TRUE(infra::LessThan()
        (4, 5));
}

TEST(LessThanTest, TestEqualsIsNotLessThanBool)
{
    EXPECT_FALSE(infra::LessThan()
        (true, true));
}

TEST(LessThanTest, TestEQualsIsNotLessThanInt)
{
    EXPECT_FALSE(infra::LessThan()
        (4, 4));
}

TEST(LessThanTest, TestLexicalOrder1)
{
    EXPECT_TRUE(infra::LessThan()
        (2, 3)
        (3, 2));
}

TEST(LessThanTest, TestLexicalOrder2)
{
    EXPECT_TRUE(infra::LessThan()
        (2, 2)
        (2, 3));
}

TEST(LessThanTest, TestLexicalOrder3)
{
    EXPECT_FALSE(infra::LessThan()
        (2, 2)
        (3, 2));
}

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
