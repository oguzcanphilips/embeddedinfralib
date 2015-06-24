#include "gtest/gtest.h"
#include "infra_util/ByteRange.hpp"
#include "infra_util/MemoryRange.hpp"

TEST(MemoryRangeTest, TestConstructedEmpty)
{
    infra::MemoryRange<int> range;

    EXPECT_TRUE(range.empty());
    EXPECT_EQ(0, range.size());
}

TEST(MemoryRangeTest, TestConstructionWithPointers)
{
    int array[] = { 0, 1, 2 };
    infra::MemoryRange<int> range(array, array + 3);

    EXPECT_FALSE(range.empty());
    EXPECT_EQ(3, range.size());
    EXPECT_EQ(&array[0], range.begin());
}

TEST(MemoryRangeTest, TestCopyConstruction)
{
    int array[] = { 0, 1, 2 };
    infra::MemoryRange<int> range(array, array + 3);
    infra::MemoryRange<const int> copy(range);

    EXPECT_FALSE(copy.empty());
    EXPECT_EQ(3, copy.size());
    EXPECT_EQ(&array[0], copy.begin());
}

TEST(MemoryRangeTest, TestConstructionWithArray)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);

    EXPECT_FALSE(range.empty());
    EXPECT_EQ(3, range.size());
    EXPECT_EQ(&array[0], range.begin());
}

TEST(MemoryRangeTest, TestConstructionWithVector)
{
    std::vector<int> vector = { 0, 1, 2 };
    infra::MemoryRange<int> range(vector);

    EXPECT_FALSE(range.empty());
    EXPECT_EQ(3, range.size());
    EXPECT_EQ(&vector[0], range.begin());
}

TEST(MemoryRangeTest, TestEnd)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);

    EXPECT_EQ(range.begin() + 3, range.end());
}

TEST(MemoryRangeTest, TestIndex)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);

    EXPECT_EQ(0, range[0]);
    EXPECT_EQ(1, range[1]);
    EXPECT_EQ(2, range[2]);
}

TEST(MemoryRangeTest, TestFront)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);

    EXPECT_EQ(0, range.front());
}

TEST(MemoryRangeTest, TestBack)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);

    EXPECT_EQ(2, range.back());
}

TEST(MemoryRangeTest, TestClear)
{
    std::array<int, 3> array = { 0, 1, 2 };
    infra::MemoryRange<int> range(array);
    range.clear();

    EXPECT_TRUE(range.empty());
}

TEST(MemoryRangeTest, TestPopFront)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.pop_front();
    EXPECT_EQ(1, range.front());
}

TEST(MemoryRangeTest, TestPopBack)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.pop_back();
    EXPECT_EQ(2, range.back());
}

TEST(MemoryRangeTest, TestShrinkFromFront)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.shrink_from_front_to(3);
    EXPECT_EQ((std::array<int, 3>{ 1, 2, 3 }), range);
}

TEST(MemoryRangeTest, TestShrinkFromFrontDoesNotShrink)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.shrink_from_front_to(6);
    EXPECT_EQ((std::array<int, 4>{ 0, 1, 2, 3 }), range);
}

TEST(MemoryRangeTest, TestShrinkFromBack)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.shrink_from_back_to(3);
    EXPECT_EQ((std::array<int, 3>{ 0, 1, 2 }), range);
}

TEST(MemoryRangeTest, TestShrinkFromBackDoesNotShrink)
{
    std::array<int, 4> array = { 0, 1, 2, 3 };
    infra::MemoryRange<int> range(array);

    range.shrink_from_back_to(6);
    EXPECT_EQ((std::array<int, 4>{ 0, 1, 2, 3 }), range);
}

TEST(MemoryRangeTest, Copy)
{
    int x = 0x12345678;
    int y;

    infra::Copy(infra::MakeByteRange(x), infra::MakeByteRange(y));
    EXPECT_EQ(0x12345678, y);
}

TEST(MemoryRangeTest, ContentsEqual)
{
    int a = 0x12;
    int x = 0x12345678;
    int y = x;
    int z = x - 1;

    EXPECT_TRUE(ContentsEqual(infra::MakeByteRange(x), infra::MakeByteRange(y)));
    EXPECT_FALSE(ContentsEqual(infra::MakeByteRange(x), infra::MakeByteRange(z)));
    EXPECT_FALSE(ContentsEqual(infra::MakeByteRange(x), infra::MakeByteRange(a)));
}
