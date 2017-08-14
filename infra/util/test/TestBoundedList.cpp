#include "gtest/gtest.h"
#include "infra/util/BoundedList.hpp"
#include "infra/util/test_helper/MoveConstructible.hpp"
#include <functional>
#include <memory>

TEST(BoundedList, TestConstructedEmpty)
{
    infra::BoundedList<int>::WithMaxSize<5> list;

    EXPECT_TRUE(list.empty());
    EXPECT_FALSE(list.full());
    EXPECT_EQ(0, list.size());
    EXPECT_EQ(5, list.max_size());
}

TEST(BoundedList, TestConstructionWith2Elements)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(2), 4);

    EXPECT_FALSE(list.empty());
    EXPECT_FALSE(list.full());
    EXPECT_EQ(2, list.size());

    EXPECT_EQ(4, *list.begin());
    EXPECT_EQ(4, *std::next(list.begin()));
}

TEST(BoundedList, TestConstructionWithRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    EXPECT_EQ(3, list.size());

    EXPECT_EQ(0, *list.begin());
    EXPECT_EQ(1, *std::next(list.begin()));
    EXPECT_EQ(2, *std::next(list.begin(), 2));
}

TEST(BoundedList, TestConstructionWithInitializerList)
{
    infra::BoundedList<int>::WithMaxSize<5> list({ 0, 1, 2 });

    EXPECT_EQ(3, list.size());

    EXPECT_EQ(0, *list.begin());
    EXPECT_EQ(1, *std::next(list.begin()));
    EXPECT_EQ(2, *std::next(list.begin(), 2));
}

TEST(BoundedList, TestCopyConstruction)
{
    infra::BoundedList<int>::WithMaxSize<5> original(std::size_t(2), 4);
    infra::BoundedList<int>::WithMaxSize<5> copy(original);

    EXPECT_EQ(2, copy.size());
    EXPECT_EQ(4, copy.front());
}

TEST(BoundedList, TestMoveConstruction)
{
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> original;
    original.emplace_front(2);
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> copy(std::move(original));

    EXPECT_EQ(1, copy.size());
    EXPECT_EQ(2, copy.front().x);
}

TEST(BoundedList, TestAssignment)
{
    infra::BoundedList<int>::WithMaxSize<5> original(std::size_t(2), 4);
    infra::BoundedList<int>::WithMaxSize<5> copy;
    copy = original;

    EXPECT_EQ(2, copy.size());
    EXPECT_EQ(4, copy.front());
}

TEST(BoundedList, TestSelfAssignment)
{
    infra::BoundedList<int>::WithMaxSize<5> original(std::size_t(2), 4);
    original = original;

    EXPECT_EQ(2, original.size());
    EXPECT_EQ(4, original.front());
}

TEST(BoundedList, TestMove)
{
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> original;
    original.emplace_front(2);
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> copy;
    copy = std::move(original);

    EXPECT_EQ(1, copy.size());
    EXPECT_EQ(2, copy.front().x);
}

TEST(BoundedList, TestBeginAndEnd)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    EXPECT_EQ(list.end(), std::next(list.begin(), 3));
    EXPECT_EQ(list.cbegin(), list.begin());
    EXPECT_EQ(list.cend(), list.end());

    EXPECT_EQ(0, *list.begin());
}

TEST(BoundedList, TestFull)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(5), 4);

    EXPECT_TRUE(list.full());
}

TEST(BoundedList, TestFront)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    EXPECT_EQ(0, list.front());
    EXPECT_EQ(0, static_cast<const infra::BoundedList<int>&>(list).front());
}

TEST(BoundedList, TestBack)
{
    int range[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    EXPECT_EQ(2, list.back());
    EXPECT_EQ(2, static_cast<const infra::BoundedList<int>&>(list).back());
}

TEST(BoundedList, TestAssignRange)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    int otherRange[2] = { 4, 5 };
    list.assign(otherRange, otherRange + 2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(4, list.front());
    EXPECT_EQ(5, *std::next(list.begin()));
}

TEST(BoundedList, TestAssignN)
{
    int range[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list(range, range + 3);

    list.assign(std::size_t(2), 4);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(4, list.front());
    EXPECT_EQ(4, *std::next(list.begin()));
}

TEST(BoundedList, TestMoveFromRange)
{
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> original;
    original.emplace_front(2);
    infra::BoundedList<infra::MoveConstructible>::WithMaxSize<5> copy;
    copy.move_from_range(original.begin(), original.end());

    EXPECT_EQ(1, copy.size());
    EXPECT_EQ(2, copy.front().x);
}

TEST(BoundedList, TestPushFront)
{
    infra::BoundedList<int>::WithMaxSize<5> list;
    int i(1);
    list.push_front(i);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, list.front());

    ++i;
    list.push_front(i);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.front());
}

TEST(BoundedList, TestPushFrontRvalue)
{
    infra::BoundedList<int>::WithMaxSize<5> list;
    list.push_front(1);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, list.front());

    list.push_front(2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.front());
}

TEST(BoundedList, TestEmplaceFront)
{
    infra::BoundedList<int>::WithMaxSize<5> list;
    list.emplace_front(1);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, list.front());

    list.emplace_front(2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.front());
}

TEST(BoundedList, TestPopFront)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(2), 4);
    list.pop_front();

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(4, list.front());

    list.pop_front();

    EXPECT_TRUE(list.empty());
}

TEST(BoundedList, TestPushBack)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(2), 4);
    list.push_back(1);

    EXPECT_EQ(3, list.size());
    EXPECT_EQ(1, list.back());
}

TEST(BoundedList, TestEmplaceBack)
{
    infra::BoundedList<int>::WithMaxSize<5> list;
    list.emplace_back(1);

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, list.back());

    list.emplace_back(2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.back());
}

TEST(BoundedList, TestPopBack)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(2), 4);
    list.pop_front();

    EXPECT_EQ(1, list.size());
    EXPECT_EQ(4, list.back());
}

TEST(BoundedList, TestSwap)
{
    int range1[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);
    int range2[3] = { 3, 4, 5};
    infra::BoundedList<int>::WithMaxSize<5> list2(range2, range2 + 3);

    swap(list1, list2);

    infra::BoundedList<int>::WithMaxSize<5> expectedList1(range2, range2 + 3);
    infra::BoundedList<int>::WithMaxSize<5> expectedList2(range1, range1 + 3);
    EXPECT_EQ(expectedList1, list1);
    EXPECT_EQ(expectedList2, list2);
}

TEST(BoundedList, TestSwapDifferentSizes)
{
    int range1[3] = { 0, 1, 2};
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);
    int range2[2] = { 3, 4};
    infra::BoundedList<int>::WithMaxSize<5> list2(range2, range2 + 2);

    swap(list1, list2);

    infra::BoundedList<int>::WithMaxSize<5> expectedList1(range2, range2 + 2);
    infra::BoundedList<int>::WithMaxSize<5> expectedList2(range1, range1 + 3);
    EXPECT_EQ(expectedList1, list1);
    EXPECT_EQ(expectedList2, list2);
}

TEST(BoundedList, TestClear)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(2), 4);
    list.clear();

    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());

    infra::BoundedList<int>::WithMaxSize<5> expectedList;
    EXPECT_EQ(expectedList, list);
}

TEST(BoundedList, TestInsert)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(1), 4);
    int i(2);
    list.insert(list.begin(), i);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.front());

    ++i;
    list.insert(std::next(list.begin()), i);

    EXPECT_EQ(3, list.size());
    EXPECT_EQ(3, *std::next(list.begin()));

    ++i;
    list.insert(list.end(), i);

    EXPECT_EQ(4, list.size());
    EXPECT_EQ(4, list.back());
}

TEST(BoundedList, TestInsertRvalue)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(1), 4);
    list.insert(list.begin(), 2);

    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, list.front());

    list.insert(std::next(list.begin()), 3);

    EXPECT_EQ(3, list.size());
    EXPECT_EQ(3, *std::next(list.begin()));

    list.insert(list.end(), 4);

    EXPECT_EQ(4, list.size());
    EXPECT_EQ(4, list.back());
}

TEST(BoundedList, TestErase)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(3), 4);
    list.erase(std::next(list.begin()));
    EXPECT_EQ(2, list.size());
    EXPECT_EQ(2, std::distance(list.begin(), list.end()));

    list.erase(std::prev(list.end()));
    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, std::distance(list.begin(), list.end()));

    list.erase(list.begin());
    EXPECT_EQ(0, list.size());
    EXPECT_EQ(0, std::distance(list.begin(), list.end()));
}

TEST(BoundedList, TestEraseAllAfter)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(3), 4);

    list.erase_all_after(list.begin());
    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, std::distance(list.begin(), list.end()));
}

TEST(BoundedList, TestEraseAllAfterNothing)
{
    infra::BoundedList<int>::WithMaxSize<5> list(std::size_t(1), 4);

    list.erase_all_after(list.begin());
    EXPECT_EQ(1, list.size());
    EXPECT_EQ(1, std::distance(list.begin(), list.end()));
}

TEST(BoundedList, TestEquals)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);
    int range2[3] = { 3, 4, 5 };
    infra::BoundedList<int>::WithMaxSize<5> list2(range2, range2 + 3);
    int range3[2] = { 0, 1 };
    infra::BoundedList<int>::WithMaxSize<5> deque3(range3, range3 + 2);

    EXPECT_TRUE(list1 == list1);
    EXPECT_FALSE(list1 == list2);
    EXPECT_FALSE(list1 == deque3);
    EXPECT_FALSE(deque3 == list1);
}

TEST(BoundedList, TestUnequals)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);

    EXPECT_FALSE(list1 != list1);
}

TEST(BoundedList, TestLessThan)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);
    int range2[3] = { 0, 4, 5 };
    infra::BoundedList<int>::WithMaxSize<5> list2(range2, range2 + 3);
    int range3[2] = { 0, 1 };
    infra::BoundedList<int>::WithMaxSize<5> deque3(range3, range3 + 2);

    EXPECT_TRUE(list1 < list2);
    EXPECT_FALSE(list1 < deque3);
    EXPECT_FALSE(list1 < list1);
}

TEST(BoundedList, TestGreaterThan)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);

    EXPECT_FALSE(list1 > list1);
}

TEST(BoundedList, TestLessThanOrEqual)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);

    EXPECT_TRUE(list1 <= list1);
}

TEST(BoundedList, TestGreaterThanOrEqual)
{
    int range1[3] = { 0, 1, 2 };
    infra::BoundedList<int>::WithMaxSize<5> list1(range1, range1 + 3);

    EXPECT_TRUE(list1 >= list1);
}
