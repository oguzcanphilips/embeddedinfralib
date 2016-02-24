#include "gtest/gtest.h"
#include "infra/util/public/BoundedVector.hpp"
#include "infra/util/test_helper/public/MoveConstructible.hpp"
#include <functional>
#include <memory>

TEST(BoundedVectorTest, TestConstructedEmpty)
{
    infra::BoundedVector<int, 5> vector;

    EXPECT_TRUE(vector.empty());
    EXPECT_FALSE(vector.full());
    EXPECT_EQ(0, vector.size());
    EXPECT_EQ(5, vector.max_size());
}

TEST(BoundedVectorTest, TestConstructionWith2Elements)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);

    EXPECT_FALSE(vector.empty());
    EXPECT_FALSE(vector.full());
    EXPECT_EQ(2, vector.size());

    EXPECT_EQ(4, vector[0]);
    EXPECT_EQ(4, vector[1]);
}

TEST(BoundedVectorTest, TestConstructionWithRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    EXPECT_EQ(3, vector.size());

    EXPECT_EQ(0, vector[0]);
    EXPECT_EQ(1, vector[1]);
    EXPECT_EQ(2, vector[2]);
}

TEST(BoundedVectorTest, TestConstructionWithInitializerList)
{
    int range[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector{ 0, 1, 2 };

    EXPECT_EQ(3, vector.size());

    EXPECT_EQ(0, vector[0]);
    EXPECT_EQ(1, vector[1]);
    EXPECT_EQ(2, vector[2]);
}

TEST(BoundedVectorTest, TestCopyConstruction)
{
    infra::BoundedVector<int, 5> original(std::size_t(2), 4);
    infra::BoundedVector<int, 5> copy(original);

    EXPECT_EQ(2, copy.size());
    EXPECT_EQ(4, copy[0]);
}

TEST(BoundedVectorTest, TestMoveConstruction)
{
    infra::BoundedVector<infra::MoveConstructible, 5> original;
    original.emplace_back(2);
    infra::BoundedVector<infra::MoveConstructible, 5> copy(std::move(original));

    EXPECT_EQ(1, copy.size());
    EXPECT_EQ(2, copy[0].x);
}

TEST(BoundedVectorTest, TestAssignment)
{
    infra::BoundedVector<int, 5> original(std::size_t(2), 4);
    infra::BoundedVector<int, 5> copy;
    copy = original;

    EXPECT_EQ(2, copy.size());
    EXPECT_EQ(4, copy[0]);
}

TEST(BoundedVectorTest, TestMove)
{
    infra::BoundedVector<infra::MoveConstructible, 5> original;
    original.emplace_back(2);
    infra::BoundedVector<infra::MoveConstructible, 5> copy;
    copy = std::move(original);

    EXPECT_EQ(1, copy.size());
    EXPECT_EQ(2, copy[0].x);
}

TEST(BoundedVectorTest, TestBeginAndEnd)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    EXPECT_EQ(vector.end(), vector.begin() + 3);
    EXPECT_EQ(vector.cbegin(), vector.begin());
    EXPECT_EQ(vector.cend(), vector.end());
    EXPECT_EQ(vector.rend(), vector.rbegin() + 3);

    EXPECT_EQ(0, *vector.begin());
    EXPECT_EQ(2, *vector.rbegin());
}

TEST(BoundedVectorTest, TestResizeBigger)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);
    vector.resize(3, 5);

    EXPECT_EQ(3, vector.size());
    EXPECT_EQ(4, vector[0]);
    EXPECT_EQ(5, vector[2]);
}

TEST(BoundedVectorTest, TestResizeSmaller)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);
    vector.resize(1, 5);

    EXPECT_EQ(1, vector.size());
    EXPECT_EQ(4, vector[0]);
}

TEST(BoundedVectorTest, TestFull)
{
    infra::BoundedVector<int, 5> vector(std::size_t(5), 4);

    EXPECT_TRUE(vector.full());
}

TEST(BoundedVectorTest, TestFront)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    EXPECT_EQ(0, vector.front());
}

TEST(BoundedVectorTest, TestBack)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    EXPECT_EQ(2, vector.back());
}

TEST(BoundedVectorTest, TestData)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);

    EXPECT_EQ(vector.begin(), vector.data());
}

TEST(BoundedVectorTest, TestAssignRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    int otherRange[2] = { 4, 5 };
    vector.assign(otherRange, otherRange + 2);

    EXPECT_EQ(2, vector.size());
    EXPECT_EQ(4, vector[0]);
    EXPECT_EQ(5, vector[1]);
}

TEST(BoundedVectorTest, TestAssignN)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.assign(std::size_t(2), 4);

    EXPECT_EQ(2, vector.size());
    EXPECT_EQ(4, vector[0]);
    EXPECT_EQ(4, vector[1]);
}

TEST(BoundedVectorTest, TestPushBack)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);
    vector.push_back(1);

    EXPECT_EQ(3, vector.size());
    EXPECT_EQ(1, vector[2]);
}

TEST(BoundedVectorTest, TestPopBack)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);
    vector.pop_back();

    EXPECT_EQ(1, vector.size());
}

TEST(BoundedVectorTest, TestInsertOneInTheMiddle)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.insert(vector.begin() + 1, 5);

    int expectedRange[4] = { 0, 5, 1, 2 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 4);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestInsertOneAtTheEnd)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.insert(vector.end(), 5);

    int expectedRange[4] = { 0, 1, 2, 5 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 4);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestInsertManyInTheMiddle)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.insert(vector.begin() + 2, std::size_t(2), 5);

    int expectedRange[5] = { 0, 1, 5, 5, 2 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 5);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestInsertManyAtTheFront)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.insert(vector.begin(), std::size_t(2), 5);

    int expectedRange[5] = { 5, 5, 0, 1, 2 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 5);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestInsertRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    int insertRange[2] = { 5, 6 };
    vector.insert(vector.begin() + 2, insertRange, insertRange + 2);

    int expectedRange[5] = { 0, 1, 5, 6, 2 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 5);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestEraseOne)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.erase(vector.begin() + 1);

    int expectedRange[5] = { 0, 2 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 2);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestEraseRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector(range, range + 3);

    vector.erase(vector.begin() + 1, vector.begin() + 3);

    int expectedRange[5] = { 0 };
    infra::BoundedVector<int, 5> expectedVector(expectedRange, expectedRange + 1);
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestSwap)
{
    int range1[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);
    int range2[3] = { 3, 4, 5};
    infra::BoundedVector<int, 5> vector2(range2, range2 + 3);

    swap(vector1, vector2);

    infra::BoundedVector<int, 5> expectedVector1(range2, range2 + 3);
    infra::BoundedVector<int, 5> expectedVector2(range1, range1 + 3);
    EXPECT_EQ(expectedVector1, vector1);
    EXPECT_EQ(expectedVector2, vector2);
}

TEST(BoundedVectorTest, TestSwapDifferentSizes)
{
    int range1[3] = { 0, 1, 2};
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);
    int range2[2] = { 3, 4};
    infra::BoundedVector<int, 5> vector2(range2, range2 + 2);

    swap(vector1, vector2);

    infra::BoundedVector<int, 5> expectedVector1(range2, range2 + 2);
    infra::BoundedVector<int, 5> expectedVector2(range1, range1 + 3);
    EXPECT_EQ(expectedVector1, vector1);
    EXPECT_EQ(expectedVector2, vector2);
}

TEST(BoundedVectorTest, TestClear)
{
    infra::BoundedVector<int, 5> vector(std::size_t(2), 4);
    vector.clear();

    infra::BoundedVector<int, 5> expectedVector;
    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestEmplace)
{
    infra::BoundedVector<infra::MoveConstructible, 5> vector;
    vector.emplace_back(2);
    vector.emplace_back(3);
    vector.emplace(vector.begin() + 1, 4);

    infra::BoundedVector<infra::MoveConstructible, 5> expectedVector;
    expectedVector.emplace_back(2);
    expectedVector.emplace_back(4);
    expectedVector.emplace_back(3);

    EXPECT_EQ(expectedVector, vector);
}

TEST(BoundedVectorTest, TestEquals)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);
    int range2[3] = { 3, 4, 5 };
    infra::BoundedVector<int, 5> vector2(range2, range2 + 3);
    int range3[2] = { 0, 1 };
    infra::BoundedVector<int, 5> vector3(range3, range3 + 2);

    EXPECT_TRUE(vector1 == vector1);
    EXPECT_FALSE(vector1 == vector2);
    EXPECT_FALSE(vector1 == vector3);
    EXPECT_FALSE(vector3 == vector1);
}

TEST(BoundedVectorTest, TestUnequals)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);

    EXPECT_FALSE(vector1 != vector1);
}

TEST(BoundedVectorTest, TestLessThan)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);
    int range2[3] = { 0, 4, 5 };
    infra::BoundedVector<int, 5> vector2(range2, range2 + 3);
    int range3[2] = { 0, 1 };
    infra::BoundedVector<int, 5> vector3(range3, range3 + 2);

    EXPECT_TRUE(vector1 < vector2);
    EXPECT_FALSE(vector1 < vector3);
    EXPECT_FALSE(vector1 < vector1);
}

TEST(BoundedVectorTest, TestGreaterThan)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);

    EXPECT_FALSE(vector1 > vector1);
}

TEST(BoundedVectorTest, TestLessThanOrEqual)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);

    EXPECT_TRUE(vector1 <= vector1);
}

TEST(BoundedVectorTest, TestGreaterThanOrEqual)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedVector<int, 5> vector1(range1, range1 + 3);

    EXPECT_TRUE(vector1 >= vector1);
}
